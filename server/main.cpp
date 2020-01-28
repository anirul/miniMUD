#include <iostream>
#include <fstream>
#include "process_keyboard.h"
#include "process_game.h"

int main(int ac, char** av) 
{
	std::cout << "starting server" << std::endl;
	server::process_game pg{};
	pg.run();
	return 0;
}