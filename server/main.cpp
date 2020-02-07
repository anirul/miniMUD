#include <iostream>
#include <fstream>
#include "keyboard.h"
#include "game.h"

int main(int ac, char** av) 
{
	std::cout << "starting server" << std::endl;
	server::game pg{};
	pg.run();
	return 0;
}