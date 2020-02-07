#pragma once

#include <mutex>
#include <thread>
#include <random>
#include "../mud_lib/helper.h"

namespace server {

	class session final : public mud::game_server::Service {
	public:
		session(
			const std::map<std::int64_t, mud::player>& id_players,
			const std::chrono::duration<double> timeout =
				std::chrono::hours(1));
		grpc::Status GetToken(
			grpc::ServerContext* context,
			const mud::token_in* request,
			mud::token_out* response) override;
		grpc::Status Login(
			grpc::ServerContext* context,
			const mud::login_in* request,
			mud::login_out* response) override;
	private:
		std::int64_t GenerateNewToken() const;
		std::unordered_map<std::int64_t, mud::player> id_players_;
		std::unordered_map<std::string, std::int64_t> name_ids_;
		std::unordered_map<std::int64_t, std::chrono::system_clock::time_point> 
			token_presence_;
		std::unordered_map<std::int64_t, std::int64_t> token_ids_;
		const std::chrono::duration<double> timeout_;
	};

} // End namespace server.