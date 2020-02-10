#include "client.h"

namespace client {

	client::client(std::shared_ptr<grpc::Channel> channel) :
		stub_(mud::game_server::NewStub(channel)) {}

	grpc::Status client::GetToken(std::shared_ptr<std::int64_t> value)
	{
		grpc::ClientContext client_context;
		mud::token_in ti{};
		mud::token_out to{};
		auto status = stub_->GetToken(&client_context, ti, &to);
		if (!status.ok()) 
		{
			throw std::runtime_error("couldn't get anything!");
		}
		token_ = to.token();
		*value = token_;
		return status;
	}

	grpc::Status client::Login(
		const mud::login_in& login,
		std::shared_ptr<mud::login_out> logout)
	{
		grpc::ClientContext client_context;
		auto status = stub_->Login(&client_context, login, logout.get());
		if (!status.ok())
		{
			throw std::runtime_error("couldn't get login trough!");
		}
		return status;
	}

} // End namespace client.