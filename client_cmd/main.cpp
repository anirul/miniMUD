#include <thread>
#include <chrono>
#include "../mud_lib/helper.h"
#include "../mud_lib/keyboard.h"
#include "../mud_lib/hash.h"
#include "client.h"
#include "control.h"
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#endif
#include <grpcpp/create_channel.h>
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif

int main(int ac, char** av) 
{
	try
	{
		std::string connection_str = "localhost:4242";
		if (ac == 2)
		{
			connection_str = av[1];
		}
		std::cout << "starting client..." << std::endl;
		auto c = std::make_shared<client::client>(
			grpc::CreateChannel(
				connection_str,
				grpc::InsecureChannelCredentials()));
		client::control control(c);
		if (!control.get_token())
		{
			return -1;
		}
		while (!control.login())
		{
			std::cout << "Bad login parameters?" << std::endl;
		}
		while (!control.select_character()) {}
		// Have to wait for the timeout not to kill us.
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		while (control.play()) 
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}