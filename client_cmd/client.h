#pragma once

#include "../mud_lib/helper.h"
#include <memory>
#include <grpcpp/channel.h>

namespace client {

	class client 
	{
	public:
		client(std::shared_ptr<grpc::Channel> channel);
		grpc::Status GetToken(
			const mud::get_token_in& in,
			std::shared_ptr<mud::get_token_out> out);
		grpc::Status Login(
			const mud::login_in& in, 
			std::shared_ptr<mud::login_out> out);
		grpc::Status SelectCharacter(
			const mud::select_character_in& in,
			std::shared_ptr<mud::select_character_out> out);
		grpc::Status CreateCharacter(
			const mud::create_character_in& in,
			std::shared_ptr<mud::create_character_out> out);
		grpc::Status Play(
			const mud::play_in& in,
			std::shared_ptr<mud::play_out> out);
	private:
		std::unique_ptr<mud::game_server::Stub> stub_;
	};

} // End namespace client.