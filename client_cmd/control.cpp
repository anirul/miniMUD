#include <sstream>
#include <stdexcept>
#include "control.h"
#include "../mud_lib/hash.h"

namespace client {

	control::control(std::shared_ptr<client> c) : client_(c)	{}

	control::~control()
	{
		keyboard_.stop();
	}

	bool control::get_token()
	{
		auto token = std::make_shared<mud::get_token_out>();
		if (!client_->GetToken({}, token).ok())
		{
			std::cerr << "error in get token RPC." << std::endl;
			return false;
		} 
		else
		{
			id_token_ = token->id_token();
			std::cout << "got token : " << id_token_ << std::endl;
		}
		return true;
	}

	bool control::login()
	{
		auto log_out = std::make_shared<mud::login_out>();
		std::cout << "Enter your name    : ";
		std::string name;
		std::cin >> name;
		if (name.empty()) return false;
		std::cout << "Enter you password : ";
		std::string password;
		std::cin >> password;
		if (password.empty()) return false;
		crypto::hash pre_hash{ name + ":" + password };
		std::cout << pre_hash.get_string() << std::endl;
		crypto::hash post_hash{
			pre_hash.get_string() + ":" +
			std::to_string(id_token_) };
		mud::login_in log_in{};
		log_in.set_name(name);
		log_in.set_password_hash(post_hash.get_string());
		log_in.set_id_token(id_token_);
		if (!client_->Login(log_in, log_out).ok())
		{
			throw std::runtime_error("error in login RPC.");
		}
		if (log_out->status() == mud::login_out::SUCCESS) 
		{
			std::cout << "Success!" << std::endl;
			for (const auto& character : log_out->characters())
			{
				characters_.push_back(character);
			}
			id_player_ = log_out->id_player();
			return true;
		}
		else
		{
			return false;
		}
	}

	bool control::select_character()
	{
		for (const auto& field : characters_)
		{
			std::cout << field << std::endl;
		}
		std::cout 
			<< "Choose your character by entering the id [0 new]" 
			<< std::endl;
		std::cout << " > ";
		std::int64_t id_character = 0;
		std::cin >> id_character;
		if (id_character != 0) 
		{
			id_character_ = id_character;
			mud::select_character_in in;
			auto out = std::make_shared<mud::select_character_out>();
			in.set_id_token(id_token_);
			in.set_id_character(id_character);
			client_->SelectCharacter(in, out);
			if (out->status() != mud::select_character_out::SUCCESS)
			{
				std::cerr << "error!" << std::endl;
				return false;
			}
			std::cout
				<< "You have selected character : "
				<< id_character
				<< std::endl;
			return true;
		}
		else
		{
			std::string name;
			std::cout << "Enter the name of your character.";
			std::cout << " > ";
			std::cin >> name;
			if (name.empty())
			{
				std::cout << "no name selected." << std::endl;
				return false;
			}
			mud::create_character_in in;
			auto out = std::make_shared<mud::create_character_out>();
			client_->CreateCharacter(in, out);
		}
		return false;
	}

	bool control::play()
	{
		keyboard_.run();
		mud::play_in in{};
		auto out = std::make_shared<mud::play_out>();
		in.set_command(keyboard_.get_key());
		in.set_id_token(id_token_);
		in.set_id_player(id_player_);
		in.set_id_character(id_character_);
		client_->Play(in, out);
		if (out->status() != mud::play_out::SUCCESS)
		{
			return false;
		}
		output(out);
		return true;
	}

	void control::output(const std::shared_ptr<mud::play_out> out) 
	{
		{
			std::ostringstream oss{};
			std::for_each(
				out->tiles().begin(),
				out->tiles().end(),
				[&oss](const mud::tile& t)
			{
				oss << t;
			});
			std::string new_hash_tiles = crypto::hash(oss.str()).get_string();
			if (hash_tiles_ != new_hash_tiles)
			{
				std::cout << "tiles(" << out->tiles().size() << ")" << std::endl;
				for (const auto& tile : out->tiles())
				{
					std::cout << tile << std::endl;
				}
				hash_tiles_ = new_hash_tiles;
			}
		}
		{
			std::ostringstream oss{};
			std::for_each(
				out->characters().begin(),
				out->characters().end(),
				[&oss](const mud::character& c)
			{
				oss << c;
			});
			std::string new_hash_characters = crypto::hash(oss.str()).get_string();
			if (hash_characters_ != new_hash_characters)
			{
				std::cout
					<< "characters(" << out->characters().size() << ")"
					<< std::endl;
				for (const auto& character : out->characters())
				{
					std::cout << character << std::endl;
				}
				hash_characters_ = new_hash_characters;
			}
		}
		{
			std::ostringstream oss{};
			std::for_each(
				out->enemies().begin(),
				out->enemies().end(),
				[&oss](const mud::enemy& e)
			{
				oss << e;
			});
			std::string new_hash_enemies = crypto::hash(oss.str()).get_string();
			if (hash_enemies_ != new_hash_enemies)
			{
				std::cout
					<< "enemies(" << out->enemies().size() << ")"
					<< std::endl;
				for (const auto& enemy : out->enemies())
				{
					std::cout << enemy << std::endl;
				}
				hash_enemies_ = new_hash_enemies;
			}
		}
	}

} // End namespace client.