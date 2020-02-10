#include "../mud_lib/helper.h"
#include "../mud_lib/keyboard.h"
#include "../mud_lib/hash.h"
#include "client.h"
#include <grpcpp/create_channel.h>

int main(int ac, char** av) 
{
	try 
	{
		input::keyboard k{};
		std::cout << "getting keyboard..." << std::endl;
		k.run();
		std::cout << "starting client..." << std::endl;
		client::client c(
			grpc::CreateChannel(
				"localhost:4242",
				grpc::InsecureChannelCredentials()));
		auto token = std::make_shared<std::int64_t>(0);
		while (!*token) 
		{
			if (!c.GetToken(token).ok())
			{
				std::cerr << "error in get token rpc." << std::endl;
				return -1;
			}
		}
		auto log_out = std::make_shared<mud::login_out>();
		while (true)
		{
			std::cout << "Enter your name    : ";
			std::string name;
			std::cin >> name;
			if (name.empty()) continue;
			std::cout << "Enter you password : ";
			std::string password;
			std::cin >> password;
			if (password.empty()) continue;
			crypto::hash pre_hash{ name + ":" + password };
			std::cout << pre_hash.get_string() << std::endl;
			crypto::hash post_hash{
				pre_hash.get_string() + ":" + std::to_string(*token) };
			mud::login_in log_in{};
			log_in.set_name(name);
			log_in.set_password_hash(post_hash.get_string());
			log_in.set_token(*token);
			if (!c.Login(log_in, log_out).ok())
			{
				std::cerr << "error in login rpc." << std::endl;
				return -2;
			}
			if (log_out->status() == mud::login_out::SUCCESS) break;
		}
		std::cout << "Success!" << std::endl;
		for (const auto& field : log_out->characters())
		{
			std::cout << field << std::endl;
		}
		k.stop();
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}