#pragma once

#include <array>
#include "../protobuf_mud_lib/helper.h"

namespace server {

	class process_character 
	{
	public:
		process_character(mud::character& character) : character_(character) {}
		bool run(
			const input_t& entry,
			mud::tile& present_tile,
			std::map<mud::direction, mud::tile>& surrounding_tiles);
	protected:
		mud::character& character_;
	};

}