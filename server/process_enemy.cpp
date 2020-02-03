#include "process_enemy.h"

namespace server {

	void process_enemy::run(
		mud::enemy& enemy,
		std::map<std::int64_t, mud::tile>& id_tiles,
		std::map<std::int64_t, mud::character>& id_characters)
	{
		// Enemy is not on a valid tile.
		if (enemy.tile_id() == 0) return;
		mud::tile& current_tile = id_tiles[enemy.tile_id()];
		// Set the enemy in place on the map (if possible).
		current_tile.set_occupant_id(enemy.id());
		current_tile.set_occupant_type(mud::tile::ENEMY);
		const auto neighbour_tiles = 
			see_around_tiles(current_tile, id_tiles);
		mud::tile character_tile = {};
		mud::direction direction = get_random_direction();
		bool found = false;
		// Search if there is any target around.
		for (const auto& direction_tile : neighbour_tiles)
		{
			if (direction_tile.second.occupant_type() == 
				mud::tile::CHARACTER)
			{
				direction = direction_tile.first;
				character_tile = direction_tile.second;
				found = true;
				break;
			}
		}
		// Face the correct direction.
		if (direction != enemy.facing()) 
		{
			if (get_invert_direction(enemy.facing()) == direction) 
			{
				*enemy.mutable_facing() = 
					get_right_direction(enemy.facing());
			}
			else if (get_right_direction(enemy.facing()) == direction)
			{
				*enemy.mutable_facing() = 
					get_right_direction(enemy.facing());
			}
			else
			{
				*enemy.mutable_facing() =
					get_left_direction(enemy.facing());
			}
			return;
		}
		// Didn't found anyone so bail out.
		if (!found) return;
		const auto around_tile = around_tiles(current_tile, id_tiles);
		// Move the enemy toward the target.
		auto it = around_tile.find(direction);
		if (it != around_tile.end()) {
			switch (it->second.occupant_type()) {
			case mud::tile::CHARACTER:
				// Attack!
				if (attack_character(
					enemy,
					id_characters[it->second.occupant_id()]))
				{
					std::cout << "\a" << std::flush;
				}
				break;
			case mud::tile::EMPTY:
				move_to(enemy, around_tile.at(direction), id_tiles);
				break;
			case mud::tile::ENEMY:
				// DO NOTHING!
				break;
			}
		}
	}

	bool process_enemy::move_to(
		mud::enemy& enemy, 
		const mud::tile& tile,
		std::map<std::int64_t, mud::tile>& id_tiles)
	{
		if (tile.type() != mud::tile::EMPTY) return false;
		if (tile.occupant_type() != mud::tile::NOBODY) return false;
		if (tile.occupant_id() != 0) return false;
		mud::tile& to = id_tiles[tile.id()];
		mud::tile& previous = id_tiles[enemy.tile_id()];
		enemy.set_tile_id(to.id());
		to.set_occupant_id(enemy.id());
		to.set_occupant_type(mud::tile::ENEMY);
		previous.set_occupant_id(0);
		previous.set_occupant_type(mud::tile::NOBODY);
		return true;
	}

	bool process_enemy::attack_character(
		const mud::enemy& enemy,
		mud::character& character)
	{
		float attack_strength = .0f;
		for (const auto& attr : enemy.attributes())
		{
			if (attr.name() == mud::attribute::MELEE_POWER) 
			{
				attack_strength += attr.score();
			}
			if (attr.name() == mud::attribute::STRENGTH)
			{
				attack_strength += attr.score();
			}
		}
		if (attack_strength > .0f)
		{
			for (auto& attr : *character.mutable_attributes())
			{
				if (attr.name() == mud::attribute::LIFE)
				{
					float temporary_life = static_cast<float>(attr.score());
					if (temporary_life <= .0f)
					{
						attr.set_score(0);
						return false;
					}
					temporary_life -= attack_strength;
					if (temporary_life <= .0f)
						temporary_life = .0f;
					attr.set_score(static_cast<std::int32_t>(temporary_life));
					return true;
				}
			}
		}
		return false;
	}

} // End namespace server.
