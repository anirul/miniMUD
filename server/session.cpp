#include "session.h"

#include "../mud_lib/hash.h"

namespace server {

	grpc::Status session::GetToken(
		grpc::ServerContext* context, 
		const mud::token_in* request, 
		mud::token_out* response)
	{
		std::int64_t token = 0;
		do 
		{
			token = GenerateNewToken();
		} 
		while (token_states_.find(token) != token_states_.end());
		state state_present{};
		state_present.time_ = std::chrono::system_clock::now();
		state_present.status_ = state::status::TOKEN;
		token_states_.insert({ token,  state_present });
		response->set_token(token);
		return grpc::Status::OK;
	}

	grpc::Status session::Login(
		grpc::ServerContext* context, 
		const mud::login_in* request, 
		mud::login_out* response)
	{
		std::int64_t token = request->token();
		auto now = std::chrono::system_clock::now();
		// No one with this token is alive.
		auto& it = token_states_.find(token);
		if (it == token_states_.end())
		{
			response->set_status(mud::login_out::UNKNOWN_TOKEN);
			return grpc::Status::OK;
		}
		// This token is old.
		if (it->second.time_ + timeout_ < now)
		{
			token_states_.erase(token);
			response->set_status(mud::login_out::TOO_OLD_TOKEN);
			return grpc::Status::OK;
		}
		if (it->second.status_ != state::status::TOKEN)
		{
			response->set_status(mud::login_out::ALREADY_CONNECTED);
			return grpc::Status::OK;
		}
		// Update the presence counter.
		token_states_[token].time_ = std::chrono::system_clock::now();
		auto player = game_->get_player(request->name());
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
		response->set_status(mud::login_out::SUCCESS);
		it->second.status_ = state::status::LOGIN;
		for (auto field : player.id_characters())
		{
			std::cout << game_->get_character(field) << std::endl;
		}
		return grpc::Status::OK;
	}

	std::int64_t session::GenerateNewToken() const
	{
		static std::random_device rd{};
		static std::mt19937_64 eng{rd()};
		static std::uniform_int_distribution<std::int64_t> distr{};
		return distr(eng);
	}

	session::session(
		const std::shared_ptr<game> g,
		const std::chrono::duration<double> timeout) :	
		game_(g),
		timeout_(timeout) {}

} // End namespace server.