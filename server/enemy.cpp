#include "enemy.h"

namespace server {

	void enemy::run(std::unordered_map<std::int64_t, mud::enemy>& id_enemies)
	{
		for (auto& id_enemy : id_enemies) {
			// Enemy is not on a valid tile.
			if (id_enemy.second.tile_id() == 0) return;
			if (!game_.has_tile(id_enemy.second.tile_id())) return;
			mud::tile& current_tile = game_.get_tile(id_enemy.second.tile_id());
			// Set the enemy in place on the map (if possible).
			current_tile.set_occupant_id(id_enemy.second.id());
			current_tile.set_occupant_type(mud::tile::ENEMY);
			const auto neighbour_tiles =
				see_around_tiles(current_tile.id(), game_.get_tiles());
			mud::tile character_tile = {};
			mud::direction direction = get_random_direction();
			bool found = false;
			// Search if there is any target around.
			for (const auto& direction_tile : neighbour_tiles)
			{
				const mud::tile& tile = game_.get_tile(direction_tile.first);
				if (tile.occupant_type() == mud::tile::CHARACTER)
				{
					direction = direction_tile.second;
					character_tile = tile;
					found = true;
					break;
				}
			}
			// Face the correct direction.
			if (direction != id_enemy.second.facing())
			{
				if (get_invert_direction(id_enemy.second.facing()) == direction)
				{
					*id_enemy.second.mutable_facing() = 
						get_right_direction(id_enemy.second.facing());
				}
				else if (get_right_direction(id_enemy.second.facing()) == 
					direction)
				{
					*id_enemy.second.mutable_facing() = 
						get_right_direction(id_enemy.second.facing());
				}
				else
				{
					*id_enemy.second.mutable_facing() = 
						get_left_direction(id_enemy.second.facing());
				}
				return;
			}
			// Didn't found anyone so bail out.
			if (!found) return;
			const auto around_tile = around_tiles(
				current_tile.id(),
				game_.get_tiles());
			// Move the enemy toward the target.
			auto it = std::find_if(
#ifndef __APPLE__
				std::execution::par,
#endif
				around_tile.begin(),
				around_tile.end(),
				[&direction](const std::pair<std::int64_t, mud::direction> p)
			{
				if (p.second == direction) return true;
				return false;
			});
			if (it != around_tile.end()) {
				const auto& tile = game_.get_tile(it->first);
				switch (tile.occupant_type()) {
				case mud::tile::CHARACTER:
					// Attack!
					if (attack_character(
						game_.get_character(tile.occupant_id()),
						id_enemy.second))
					{
						std::cout << "\a" << std::flush;
					}
					break;
				case mud::tile::EMPTY:
					move_to(
						id_enemy.second.tile_id(), 
						it->first, 
						id_enemy.second);
					break;
				case mud::tile::ENEMY:
					// DO NOTHING!
					break;
				}
			}
		}
	}

	bool enemy::move_to(std::int64_t from, std::int64_t to, mud::enemy& me)
	{
		mud::tile& tile_to = game_.get_tile(to);
		if (tile_to.type() != mud::tile::EMPTY) return false;
		if (tile_to.occupant_type() != mud::tile::NOBODY) return false;
		if (tile_to.occupant_id() != 0) return false;
		mud::tile& tile_from = game_.get_tile(from);
		me.set_tile_id(tile_to.id());
		tile_to.set_occupant_id(me.id());
		tile_to.set_occupant_type(mud::tile::ENEMY);
		tile_from.set_occupant_id(0);
		tile_from.set_occupant_type(mud::tile::NOBODY);
		return true;
	}

	bool enemy::attack_character(
		mud::character& character, 
		const mud::enemy& me)
	{
		float attack_strength = .0f;
		for (const auto& attr : me.attributes())
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
