#pragma once

#include <mutex>
#include <thread>
#include <random>
#include "../mud_lib/helper.h"
#include "game.h"

namespace server {

	class session final : public mud::game_server::Service 
	{
	public:
		session(
			const std::shared_ptr<game> g,
			const std::chrono::duration<double> timeout =
				std::chrono::minutes(10));
		grpc::Status GetToken(
			grpc::ServerContext* context,
			const mud::token_in* request,
			mud::token_out* response) override;
		grpc::Status Login(
			grpc::ServerContext* context,
			const mud::login_in* request,
			mud::login_out* response) override;
		std::map<std::int64_t, mud::player> get_id_players();
	protected:
		std::int64_t GenerateNewToken() const;
	public:
		struct state 
		{
			enum class status 
			{
				TOKEN = 0,
				LOGIN = 1,
				PLAYING = 2
			};
			std::int64_t id_player_;
			std::int64_t id_character_;
			std::chrono::system_clock::time_point time_;
			status status_;
		};
	private:
		const std::shared_ptr<game> game_;
		std::unordered_map<std::int64_t, state> token_states_;
		const std::chrono::duration<double> timeout_;
	};

} // End namespace server.