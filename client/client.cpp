#include "client.h"

namespace client {

	client::client(std::shared_ptr<grpc::Channel> channel) :
		stub_(mud::game_server::NewStub(channel)) {}

	std::int64_t client::GetToken()
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
		return token_;
	}

} // End namespace client.