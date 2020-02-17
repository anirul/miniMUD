#include "client.h"

namespace client {

	client::client(std::shared_ptr<grpc::Channel> channel) :
		stub_(mud::game_server::NewStub(channel)) {}

	grpc::Status client::GetToken(
		const mud::get_token_in& in,
		std::shared_ptr<mud::get_token_out> out)
	{
		grpc::ClientContext client_context;
		auto status = stub_->GetToken(&client_context, in, out.get());
		if (!status.ok()) 
		{
			throw std::runtime_error("couldn't get token!");
		}
		return status;
	}

	grpc::Status client::Login(
		const mud::login_in& in,
		std::shared_ptr<mud::login_out> out)
	{
		grpc::ClientContext client_context;
		auto status = stub_->Login(&client_context, in, out.get());
		if (!status.ok())
		{
			throw std::runtime_error("couldn't get login!");
		}
		return status;
	}

	grpc::Status client::SelectCharacter(
		const mud::select_character_in& in, 
		std::shared_ptr<mud::select_character_out> out)
	{
		grpc::ClientContext client_context;
		auto status = stub_->SelectCharacter(&client_context, in, out.get());
		if (!status.ok())
		{
			throw std::runtime_error("couldn't get select character!");
		}
		return status;
	}

	grpc::Status client::CreateCharacter(
		const mud::create_character_in& in, 
		std::shared_ptr<mud::create_character_out> out)
	{
		grpc::ClientContext client_context;
		auto status = stub_->CreateCharacter(&client_context, in, out.get());
		if (!status.ok())
		{
			throw std::runtime_error("couldn't get create character!");
		}
		return status;
	}

	grpc::Status client::Play(
		const mud::play_in& in, 
		std::shared_ptr<mud::play_out> out)
	{
		grpc::ClientContext client_context;
		auto status = stub_->Play(&client_context, in, out.get());
		if (!status.ok())
		{
			throw std::runtime_error("couldn't get play!");
		}
		return status;
	}

} // End namespace client.