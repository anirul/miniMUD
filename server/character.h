#pragma once

#include <array>
#include "../mud_lib/helper.h"
#include "keyboard.h"

namespace server {

	class character 
	{
	public:
		character(mud::character& c) : character_(c) {}
		bool run(
			const input_t& entry,
			mud::tile& present_tile,
			std::map<mud::direction, mud::tile>& surrounding_tiles);
	protected:
		mud::character& character_;
	};

}