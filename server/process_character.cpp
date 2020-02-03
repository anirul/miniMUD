#include "process_character.h"

namespace server {

	bool process_character::run(
		const input_t& entry, 
		mud::tile& present_tile,
		std::map<mud::direction, mud::tile>& surrounding_tiles)
	{
		if (character_.state() == mud::character::NONE) return true;
		switch (entry) {
		case input_t::FORWARD:
		{
			auto& it = surrounding_tiles.find(character_.facing());
			if (it == surrounding_tiles.end()) break;
			const mud::tile& front = it->second;
			if (front.type() == mud::tile::EMPTY && 
				front.occupant_type() == mud::tile::NOBODY)
			{
				character_.set_tile_id(front.id());
			}
		}
		break;
		case input_t::BACKWARD:
		{
			auto& it = surrounding_tiles.find(
				get_invert_direction(character_.facing()));
			if (it == surrounding_tiles.end()) break;
			const mud::tile& back = it->second;
			if (back.type() == mud::tile::EMPTY &&
				back.occupant_type() == mud::tile::NOBODY)
			{
				character_.set_tile_id(back.id());
			}
		}
		break;
		case input_t::LEFT:
		{
			*character_.mutable_facing() = 
				get_left_direction(character_.facing());
		}
		break;
		case input_t::RIGHT:
		{
			*character_.mutable_facing() = 
				get_right_direction(character_.facing());
		}
		break;
		case input_t::ATTACK:
		{
			if (surrounding_tiles[character_.facing()].occupant_type() == 
				mud::tile::ENEMY)
			{
				auto id = surrounding_tiles[character_.facing()].occupant_id();
				std::cout << "\a" << std::flush;
			}
		}
		break;
		case input_t::QUIT:
		{
			return false;
		}
		break;
		case input_t::INFO:
		{
			std::cout << "Who am I" << std::endl;
			std::cout << character_;
			std::cout << "Where am I" << std::endl;
			std::cout << present_tile;
		}
		break;
		}
		return true;
	}

} // End namespace server.