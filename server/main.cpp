#include <iostream>
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

std::ostream& operator<< (std::ostream& os, const mud::player& player) 
{
	os << player.name() << std::endl;
	os << player.password_hash() << std::endl;
	os << player.id() << std::endl;
	os << "characters(" << player.id_characters().size() << ")" << std::endl;
	for (const auto& id : player.id_characters()) 
	{
		os << "\t" << id << std::endl;
	}
	return os;
}

int main(int ac, char** av) 
{
	std::cout << "starting server" << std::endl;
	mud::player player{};
	player.set_name("test");
	player.set_password_hash("dummy fake password");
	player.set_id(32);
	std::cout << player;
	return 0;
}