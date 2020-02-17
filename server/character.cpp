#include "character.h"

namespace server {

	std::unordered_map<std::int64_t, mud::play_out::status_enum> character::run(
		const std::unordered_map<std::int64_t, mud::play_in::command_enum>& 
			entries,
		std::unordered_map<std::int64_t, mud::character>& id_characters)
	{
		std::unordered_map<std::int64_t, mud::play_out::status_enum> out{};
		for (auto& id_character : id_characters)
		{
			out[id_character.first] = mud::play_out::FAILURE;
			if (id_character.second.name().empty()) continue;
			if (id_character.second.id() == 0) continue;
			if (id_character.second.state() == mud::character::NONE) continue;
			std::map<mud::direction, std::int64_t> direction_id_tile_map;
			for (const auto& field : around_tiles(
				id_character.second.tile_id(), 
				game_.get_tiles()))
			{
				direction_id_tile_map.insert({ field.second, field.first });
			}
			out[id_character.first] = mud::play_out::SUCCESS;
			if (entries.find(id_character.first) == entries.end()) continue;
			switch (entries.at(id_character.first)) 
			{
			case mud::play_in::FORWARD:
			{
				auto& it = direction_id_tile_map.find(
					id_character.second.facing());
				if (it == direction_id_tile_map.end()) continue;
				const mud::tile& front = game_.get_tile(it->second);
				if (front.type() == mud::tile::EMPTY &&
					front.occupant_type() == mud::tile::NOBODY)
				{
					std::int64_t present_tile_id = 
						id_character.second.tile_id();
					game_.get_tile(present_tile_id).set_occupant_id(0);
					game_.get_tile(present_tile_id).set_occupant_type(
						mud::tile::NOBODY);
					id_character.second.set_tile_id(front.id());
					game_.get_tile(front.id()).set_occupant_id(
						id_character.first);
					game_.get_tile(front.id()).set_occupant_type(
						mud::tile::CHARACTER);
				}
			}
			break;
			case mud::play_in::BACKWARD:
			{
				auto& it = direction_id_tile_map.find(
					get_invert_direction(id_character.second.facing()));
				if (it == direction_id_tile_map.end()) continue;
				const mud::tile& back = game_.get_tile(it->second);
				if (back.type() == mud::tile::EMPTY &&
					back.occupant_type() == mud::tile::NOBODY)
				{
					std::int64_t present_tile_id =
						id_character.second.tile_id();
					game_.get_tile(present_tile_id).set_occupant_id(0);
					game_.get_tile(present_tile_id).set_occupant_type(
						mud::tile::NOBODY);
					id_character.second.set_tile_id(back.id());
					game_.get_tile(back.id()).set_occupant_id(
						id_character.first);
					game_.get_tile(back.id()).set_occupant_type(
						mud::tile::CHARACTER);
				}
			}
			break;
			case mud::play_in::TURN_LEFT:
			{
				*id_character.second.mutable_facing() =
					get_left_direction(id_character.second.facing());
			}
			break;
			case mud::play_in::TURN_RIGHT:
			{
				*id_character.second.mutable_facing() =
					get_right_direction(id_character.second.facing());
			}
			break;
			case mud::play_in::QUIT:
			{
				out[id_character.first] = mud::play_out::QUIT;
			}
			break;
			case mud::play_in::NO_COMMAND:
			{} // Do nothing...
			break;
			default:
			{
				std::cout 
					<< entries.at(id_character.first) 
					<< " : not implemented!" 
					<< std::endl;
			}
			}
		}
		return out;
	}

} // End namespace server.