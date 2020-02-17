#pragma once

#include <array>
#include "game.h"
#include "../mud_lib/helper.h"

namespace server {

	class character 
	{
	public:
		character(game& g) : game_(g) {}
		std::unordered_map<std::int64_t, mud::play_out::status_enum> run(
			const std::unordered_map<std::int64_t, mud::play_in::command_enum>& 
				entries,
			std::unordered_map<std::int64_t, mud::character>& id_characters);
	protected:
		game& game_;
	};

}