#pragma once

#include <mutex>
#include <thread>
#include <random>
#include <unordered_map>
#include "../mud_lib/helper.h"
#include "game.h"

namespace server {

	class session final : public mud::game_server::Service 
	{
	public:
		session(
			std::shared_ptr<std::mutex> m,
			const std::shared_ptr<game> g,
			const std::chrono::duration<double> timeout =
				std::chrono::minutes(10));
		grpc::Status GetToken(
			grpc::ServerContext* context,
			const mud::get_token_in* request,
			mud::get_token_out* response) override;
		grpc::Status Login(
			grpc::ServerContext* context,
			const mud::login_in* request,
			mud::login_out* response) override;
		grpc::Status SelectCharacter(
			grpc::ServerContext* context,
			const mud::select_character_in* request,
			mud::select_character_out* response) override;
		grpc::Status CreateCharacter(
			grpc::ServerContext* context,
			const mud::create_character_in* request,
			mud::create_character_out* response) override;
		grpc::Status Play(
			grpc::ServerContext* context, 
			const mud::play_in* request,
			mud::play_out* response) override;
	protected:
		std::int64_t generate_new_token() const;
		bool is_token_ok(std::int64_t id);
		bool are_values_ok(
			std::int64_t id_token, 
			std::int64_t id_player, 
			std::int64_t id_character);
	public:
		struct state 
		{
			enum class status 
			{
				TOKEN = 0,
				LOGIN = 1,
				PLAYING = 2
			};
			std::int64_t id_player_ = 0;
			std::int64_t id_character_ = 0;
			// Get the current time.
			std::chrono::system_clock::time_point time_ = 
				std::chrono::system_clock::now();
			// Get the current iteration.
			std::int64_t iteration_ = 0;
			// Get the current status.
			status status_ = status::TOKEN;
		};
	private:
		std::shared_ptr<std::mutex> mutex_;
		const std::shared_ptr<game> game_;
		std::unordered_map<std::int64_t, state> token_state_map_;
		std::unordered_map<std::int64_t, std::int64_t> player_id_token_map_;
		std::unordered_map<std::int64_t, std::chrono::system_clock::time_point>
			player_id_time_map_;
		const std::chrono::duration<double> timeout_;
	};

} // End namespace server.