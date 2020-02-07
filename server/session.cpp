#include "session.h"

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
		while (token_presence_.find(token) != token_presence_.end());
		token_presence_.insert({ token, std::chrono::system_clock::now() });
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
		auto it_presence = token_presence_.find(token);
		if (it_presence == token_presence_.end())
		{
			response->set_status(mud::login_out::UNKNOWN_TOKEN);
			return grpc::Status::OK;
		}
		// This token is old.
		if (it_presence->second + timeout_ < now)
		{
			token_presence_.erase(token);
			token_ids_.erase(token);
			response->set_status(mud::login_out::TOO_OLD_TOKEN);
			return grpc::Status::OK;
		}
		if (token_ids_.find(token) != token_ids_.end())
		{
			response->set_status(mud::login_out::ALREADY_CONNECTED);
			return grpc::Status::OK;
		}
		// Update the presence counter.
		token_presence_[token] = std::chrono::system_clock::now();
		auto it_name = name_ids_.find(request->name());
		if (it_name == name_ids_.end())
		{
			response->set_status(mud::login_out::FAILURE);
			return grpc::Status::OK;
		}
		// TODO(dubouchet): now we should check the password.
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
		const std::map<std::int64_t, mud::player>& id_players, 
		const std::chrono::duration<double> timeout) :	
		id_players_(id_players.begin(), id_players.end()),
		timeout_(timeout) 
	{
		for (const auto& player : id_players_)
		{
			name_ids_.insert({ player.second.name(), player.first });
		}
	}

} // End namespace server.