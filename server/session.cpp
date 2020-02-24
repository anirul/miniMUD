#include "session.h"

#include "../mud_lib/hash.h"

namespace server {

	session::session(
		std::shared_ptr<std::mutex> m,
		const std::shared_ptr<game> g,
		const std::chrono::duration<double> timeout) :
		mutex_(m),
		game_(g),
		timeout_(timeout) {}

	std::int64_t session::generate_new_token() const
	{
		static std::random_device rd{};
		static std::mt19937_64 eng{ rd() };
		static std::uniform_int_distribution<std::int64_t> distr{};
		return distr(eng);
	}

	bool session::is_token_ok(std::int64_t token)
	{
		try
		{
			// Check if token exist.
			const auto& it = token_state_map_.find(token);
			if (it == token_state_map_.end())
			{
				return false;
			}
			// Check iteration is not the current one and is newer.
			std::int64_t current_iteration = [this]
			{
				std::scoped_lock l(*mutex_);
				return game_->iteration();
			}();
			if (it->second.iteration_ >= current_iteration)
			{
				return false;
			}
			// Update iterations counter.
			it->second.iteration_ = current_iteration;
			// Check time (and update it).
			auto now = std::chrono::system_clock::now();
			if (now - it->second.time_ > timeout_)
			{
				token_state_map_.erase(token);
				return false;
			}
			it->second.time_ = now;
			return true;
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
			return false;
		}
	}

	bool session::are_values_ok(
		std::int64_t id_token,
		std::int64_t id_player,
		std::int64_t id_character)
	{
		if (!is_token_ok(id_token)) {
			return false;
		}
		auto it = token_state_map_.find(id_token);
		if (id_player != it->second.id_player_)
		{
			return false;
		}
		if (id_character != it->second.id_character_)
		{
			return false;
		}
		if (it->second.status_ != state::status::PLAYING)
		{
			return false;
		}
		return true;
	}

	// TODO: Should limit the number of token you can get per frame.
	grpc::Status session::GetToken(
		grpc::ServerContext* context, 
		const mud::get_token_in* request, 
		mud::get_token_out* response)
	{
		std::string peer = context->peer();
		std::cout << "connection from : " << peer << std::endl;
		std::int64_t token = 0;
		do 
		{
			token = generate_new_token();
		} 
		while (token_state_map_.find(token) != token_state_map_.end());
		state state_present{};
		state_present.iteration_ = [this]
		{
			std::scoped_lock l(*mutex_);
			return game_->iteration();
		}();
		state_present.time_ = std::chrono::system_clock::now();
		state_present.status_ = state::status::TOKEN;
		token_state_map_.insert({ token,  state_present });
		response->set_id_token(token);
		return grpc::Status::OK;
	}

	grpc::Status session::Login(
		grpc::ServerContext* context, 
		const mud::login_in* request, 
		mud::login_out* response)
	{
		response->set_status(mud::login_out::FAILURE);
		// Probably should avoid using exception here.
		try
		{
			std::int64_t token = request->id_token();
			auto now = std::chrono::system_clock::now();
			if (!is_token_ok(token))
			{
				return grpc::Status::OK;
			}
			const auto& it = token_state_map_.find(token);
			// Should never happen.
			if (it->second.status_ != state::status::TOKEN)
			{
				response->set_status(mud::login_out::ALREADY_CONNECTED);
				return grpc::Status::OK;
			}
			const mud::player player = [this, &request] 
			{
				std::scoped_lock l(*mutex_);
				return game_->get_player(request->name());
			}();
			if (player.name().empty())
			{
				response->set_status(mud::login_out::FAILURE);
				return grpc::Status::OK;
			}
			// Now we should check the password.
			crypto::hash hash{
				player.password_hash() + ":" + std::to_string(token) };
			if (request->password_hash() != hash.get_string())
			{
				response->set_status(mud::login_out::FAILURE);
				return grpc::Status::OK;
			}
			// Check if already log in.
			const auto& it_player_time = player_id_time_map_.find(player.id());
			if (it_player_time != player_id_time_map_.end() &&
				now - it_player_time->second < timeout_)
			{
				response->set_status(mud::login_out::FAILURE);
				return grpc::Status::OK;
			}
			else
			{
				player_id_time_map_[player.id()] = 
					std::chrono::system_clock::now();
			}
			// Update the presence counter.
			token_state_map_[token].time_ = std::chrono::system_clock::now();
			player_id_token_map_[player.id()] = token;
			// Successfully log in!
			response->set_status(mud::login_out::SUCCESS);
			it->second.status_ = state::status::LOGIN;
			it->second.id_player_ = player.id();
			for (auto field : player.id_characters())
			{
				std::scoped_lock l(*mutex_);
				*response->add_characters() = game_->get_character(field);
			}
			response->set_id_player(player.id());
		}
		catch (const std::exception& ex)
		{
			std::cerr << ex.what() << std::endl;
			return grpc::Status::CANCELLED;
		}
		return grpc::Status::OK;
	}

	grpc::Status session::SelectCharacter(
		grpc::ServerContext* context, 
		const mud::select_character_in* request, 
		mud::select_character_out* response)
	{
		response->set_status(mud::select_character_out::FAILURE);
		if (!is_token_ok(request->id_token()))
		{
			return grpc::Status::OK;
		}
		const auto& it = token_state_map_.find(request->id_token());
		// Probably should avoid using exception here.
		try 
		{
			std::scoped_lock l(*mutex_);
			// Check if player has this character.
			mud::player player = game_->get_player(it->second.id_player_);
			if (player.name().empty())
			{
				// Player has no name -> don't exist -> out!
				return grpc::Status::CANCELLED;
			}
			// Check id player has a character that correspond.
			if (!game_->has_player_character(
				it->second.id_player_, 
				request->id_character()))
			{
				return grpc::Status::CANCELLED;
			}
		}
		catch (const std::exception& ex) 
		{
			std::cerr << ex.what() << std::endl;
			return grpc::Status::CANCELLED;
		}
		it->second.id_character_ = request->id_character();
		it->second.status_ = state::status::PLAYING;
		game_->activate_character(request->id_character());
		response->set_status(mud::select_character_out::SUCCESS);
		return grpc::Status::OK;
	}

	grpc::Status session::CreateCharacter(
		grpc::ServerContext* context, 
		const mud::create_character_in* request, 
		mud::create_character_out* response)
	{
		response->set_status(mud::create_character_out::FAILURE);
		if (!is_token_ok(request->id_token()))
		{
			return grpc::Status::OK;
		}
		const auto& it = token_state_map_.find(request->id_token());
		std::int64_t id = 0;
		{
			std::scoped_lock l(*mutex_);
			id = game_->create_new_character(
				request->name(),
				it->second.id_player_);
		}
		if (id == 0) {
			return grpc::Status::OK;
		}
		response->set_id_character(id);
		response->set_status(mud::create_character_out::SUCCESS);
		{
			std::scoped_lock l(*mutex_);
			for (const auto& field : game_->get_characters())
			{
				*response->add_characters() = field.second;
			}
		}
		it->second.status_ = state::status::LOGIN;
		return grpc::Status::OK;
	}

	grpc::Status session::Play(
		grpc::ServerContext* context,
		const mud::play_in* request,
		mud::play_out* response)
	{
		// Check the token.
		response->set_status(mud::play_out::FAILURE);
		if (!are_values_ok(
			request->id_token(), 
			request->id_player(), 
			request->id_character()))
		{
			return grpc::Status::OK;
		}
		{
			std::scoped_lock l(*mutex_);
			game_->add_command(request->id_character(), request->command());
			*response = game_->get_play_out(request->id_character());
		}
		return grpc::Status::OK;
	}

} // End namespace server.
