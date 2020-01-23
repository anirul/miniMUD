#include "process_keyboard.h"

input server::process_keyboard::run()
{
	if (GetAsyncKeyState(VK_UP) < 0) 
	{
		std::cout << "UP" << std::endl;
		return input::FORWARD;
	}
	if (GetAsyncKeyState(VK_DOWN) < 0) 
	{
		std::cout << "DOWN" << std::endl;
		return input::BACKWARD;
	}
	if (GetAsyncKeyState(VK_LEFT) < 0) 
	{
		std::cout << "LEFT" << std::endl;
		return input::LEFT;
	}
	if (GetAsyncKeyState(VK_RIGHT) < 0) 
	{
		std::cout << "RIGHT" << std::endl;
		return input::RIGHT;
	}
	if (GetAsyncKeyState(VK_ESCAPE) < 0) 
	{
		std::cout << "ESC" << std::endl;
		return input::QUIT;
	}
	if (GetAsyncKeyState(VK_TAB) < 0) 
	{
		std::cout << "TAB" << std::endl;
		return input::INFO;
	}
	if (GetAsyncKeyState(VK_RETURN) < 0) 
	{
		std::cout << "RETURN" << std::endl;
		return input::ATTACK;
	}
	if (GetAsyncKeyState(VK_SNAPSHOT) < 0)
	{
		std::cout << "PRINT" << std::endl;
		return input::PRINT;
	}
	return input::NONE;
}
