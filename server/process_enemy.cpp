#include "process_enemy.h"
#include "../protobuf_mud_lib/helper.h"

namespace server {

	void process_enemy::run() 
	{
		for (auto& field : id_enemies_) {
			mud::enemy& enemy = field.second;
			// Enemy is not on a valid tile.
			if (enemy.tile_id() == 0) continue;
			mud::tile& current_tile = id_tiles_[enemy.tile_id()];
			const auto neighbour_tiles = 
				see_around_tiles(current_tile, id_tiles_);
			mud::tile character_tile = {};
			mud::direction direction = get_random_direction();
			bool found = false;
			// Search if there is any target around.
			for (const auto& direction_tile : neighbour_tiles)
			{
				if (direction_tile.second.occupant_type() == mud::CHARACTER)
				{
					direction = direction_tile.first;
					character_tile = direction_tile.second;
					found = true;
					break;
				}
			}
			// Face the correct direction.
			if (direction != enemy.facing()) {
				if (get_invert_direction(enemy.facing()) == direction) {
					enemy.set_facing(get_right_direction(enemy.facing()));
				}
				else if (get_right_direction(enemy.facing()) == direction)
				{
					enemy.set_facing(get_right_direction(enemy.facing()));
				}
				else
				{
					enemy.set_facing(get_left_direction(enemy.facing()));
				}
				continue;
			}
			// Didn't found anyone so bail out.
			if (!found) continue;
			const auto around_tile = around_tiles(current_tile, id_tiles_);
			// Move the enemy toward the target.
			if (around_tile.at(direction).occupant_type() != mud::CHARACTER)
			{
				move_to(enemy, around_tile.at(direction));
				continue; 
			}
			// TODO attack!
			std::cout << "\a" << std::flush;
		}
	}

	void process_enemy::move_to(const mud::enemy& enemy, const mud::tile& tile)
	{
		if (tile.type() != mud::EMPTY) return;
		if (tile.occupant_type() != mud::NONE) return;
		mud::enemy& me = id_enemies_[enemy.id()];
		mud::tile& there = id_tiles_[tile.id()];
		me.set_tile_id(there.id());
		there.set_occupant_id(me.id());
		there.set_occupant_type(mud::ENEMY);
	}

} // End namespace server.