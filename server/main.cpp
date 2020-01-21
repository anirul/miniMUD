#include <iostream>
#include <fstream>
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#endif
#include "../protobuf_mud_lib/mud_lib.pb.h"
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif
#include "process_game.h"

int main(int ac, char** av) 
{
	std::cout << "starting server" << std::endl;
	server::process_game pg{};
	pg.run();
	return 0;
}