#include "../mud_lib/helper.h"
#include "../mud_lib/keyboard.h"
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
		std::int64_t token = 0;
		while (!k.check_released_input(input::input_t::QUIT)) {
			std::int64_t new_token = c.GetToken();
			if (token != new_token)
			{
				std::cout << c.GetToken() << std::endl;
			}
			token = new_token;
		}
		k.stop();
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}