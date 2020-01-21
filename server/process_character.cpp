#include "process_character.h"
#include "../protobuf_mud_lib/helper.h"

namespace server {

	bool process_character::run(
		input entry, 
		mud::tile& present_tile,
		std::map<mud::direction, mud::tile>& surrounding_tiles)
	{
		if (character_.state() == mud::NONE) return true;
		switch (entry) {
		case input::FORWARD:
		{
			auto& it = surrounding_tiles.find(character_.facing());
			if (it == surrounding_tiles.end()) break;
			const mud::tile& front = it->second;
			if (front.type() == mud::EMPTY && 
				front.occupant_type() == mud::NOBODY)
			{
				character_.set_tile_id(front.id());
			}
		}
		break;
		case input::BACKWARD:
		{
			auto& it = surrounding_tiles.find(
				get_invert_direction(character_.facing()));
			if (it == surrounding_tiles.end()) break;
			const mud::tile& back = it->second;
			if (back.type() == mud::EMPTY &&
				back.occupant_type() == mud::NOBODY)
			{
				character_.set_tile_id(back.id());
			}
		}
		break;
		case input::LEFT:
		{
			character_.set_facing(get_left_direction(character_.facing()));
		}
		break;
		case input::RIGHT:
		{
			character_.set_facing(get_right_direction(character_.facing()));
		}
		break;
		case input::ATTACK:
		{
			// TODO make an attack on the character that is in front me.
		}
		break;
		case input::QUIT:
		{
			return false;
		}
		break;
		case input::INFO:
		{
			std::cout << character_;
		}
		break;
		}
		return true;
	}

} // End namespace server.