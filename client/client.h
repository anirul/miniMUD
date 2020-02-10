#pragma once

#include "../mud_lib/helper.h"
#include <memory>
#include <grpcpp/channel.h>

namespace client {

	class client 
	{
	public:
		client(std::shared_ptr<grpc::Channel> channel);
		grpc::Status GetToken(std::shared_ptr<std::int64_t> value);
		grpc::Status Login(
			const mud::login_in& login, 
			std::shared_ptr<mud::login_out> logout);
	private:
		std::int64_t token_ = 0;
		std::unique_ptr<mud::game_server::Stub> stub_;
	};

} // End namespace client.