#pragma once

#include <array>
#include "../mud_lib/helper.h"
#include "../mud_lib/keyboard.h"

namespace server {

	class character 
	{
	public:
		character(mud::character& c) : character_(c) {}
		bool run(
			const input::input_t& entry,
			mud::tile& present_tile,
			std::map<mud::direction, mud::tile>& surrounding_tiles);
	protected:
		mud::character& character_;
	};

}