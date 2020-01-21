#include "process_game.h"

#include "../protobuf_mud_lib/helper.h"
#include "process_character.h"
#include "process_enemy.h"
#include "process_keyboard.h"
#include "process_enemy.h"

namespace server {

	process_game::process_game(
		double total_time,
		const std::string& player_file,
		const std::string& character_file,
		const std::string& enemy_file,
		const std::string& tile_file) :
		total_time_(total_time),
		player_file_(player_file),
		character_file_(character_file),
		enemy_file_(enemy_file),
		tile_file_(tile_file)
	{
		{
			mud::player_book book = read_file<mud::player_book>(player_file_);
			std::for_each(
				book.players().begin(),
				book.players().end(),
				[this](const mud::player& player) 
			{
				id_players_.insert({ player.id(), player });
			});
		}
		{
			mud::character_book book = read_file<mud::character_book>(
				character_file_);
			std::for_each(
				book.characters().begin(),
				book.characters().end(),
				[this](const mud::character& character) 
			{
				id_characters_.insert({ character.id(), character });
			});
		}
		{
			mud::enemy_book book = read_file<mud::enemy_book>(enemy_file_);
			std::for_each(
				book.enemies().begin(),
				book.enemies().end(),
				[this](const mud::enemy& enemy) 
			{
				id_enemies_.insert({ enemy.id(), enemy });
			});
		}
		{
			mud::tile_book book = read_file<mud::tile_book>(tile_file_);
			if (book.tiles().size() > 1) 
			{
				std::for_each(
					book.tiles().begin(),
					book.tiles().end(),
					[this](const mud::tile& tile)
				{
					id_tiles_.insert({ tile.id(), tile });
				});
			}
			else
			{
				// Fill up the tiles in case there is nothing.
				{
					mud::tile t0{};
					t0.set_mood("this is tile 0");
					t0.set_id(0);
					t0.set_type(mud::TREE);
					id_tiles_.insert({ 0, t0 });
				}
				{
					mud::tile t1{};
					t1.set_mood("this is the starting tile!");
					t1.set_id(1);
					t1.set_type(mud::EMPTY);
					mud::location t2_location{};
					t2_location.set_direction(mud::EAST);
					t2_location.set_id(2);
					mud::location t3_location{};
					t3_location.set_direction(mud::SOUTH);
					t3_location.set_id(3);
					*t1.add_neighbours() = t2_location;
					*t1.add_neighbours() = t3_location;
					id_tiles_.insert({ 1, t1 });
				}
				{
					mud::tile t2{};
					t2.set_mood("this is a tile on the right of t1");
					t2.set_id(2);
					t2.set_type(mud::EMPTY);
					mud::location t1_location{};
					t1_location.set_direction(mud::WEST);
					t1_location.set_id(1);
					*t2.add_neighbours() = t1_location;
					id_tiles_.insert({ 2, t2 });
				}
				{
					mud::tile t3{};
					t3.set_mood("this is an edge");
					t3.set_id(3);
					t3.set_type(mud::EMPTY);
					mud::location t1_location{};
					t1_location.set_direction(mud::NORTH);
					t1_location.set_id(1);
					*t3.add_neighbours() = t1_location;
					id_tiles_.insert({ 3, t3 });
				}
			}
		}
	}

	process_game::~process_game()
	{
		{
			mud::player_book book;
			std::for_each(
				id_players_.begin(),
				id_players_.end(),
				[&book](const std::pair<std::int64_t, mud::player>& pair)
			{
				*book.add_players() = pair.second;
			});
			write_file(player_file_, book);
		}
		{
			mud::character_book book;
			std::for_each(
				id_characters_.begin(),
				id_characters_.end(),
				[&book](const std::pair<std::int64_t, mud::character>& pair)
			{
				*book.add_characters() = pair.second;
			});
			write_file(character_file_, book);
		}
		{
			mud::enemy_book book;
			std::for_each(
				id_enemies_.begin(),
				id_enemies_.end(),
				[&book](const std::pair<std::int64_t, mud::enemy>& pair)
			{
				*book.add_enemies() = pair.second;
			});
			write_file(enemy_file_, book);
		}
		{
			mud::tile_book book;
			std::for_each(
				id_tiles_.begin(),
				id_tiles_.end(),
				[&book](const std::pair<std::int64_t, mud::tile>& pair)
			{
				*book.add_tiles() = pair.second;
			});
			write_file(tile_file_, book);
		}

	}

	void process_game::run()
	{
		bool running = true;
		bool has_valid_character = false;
		std::int64_t current_tile_id = 0;
		mud::tile& current_tile = mud::tile{};
		input entry = input::NONE;
		process_enemy pe(id_enemies_, id_tiles_);
		while (running) 
		{
			auto start_time = std::chrono::high_resolution_clock::now();
			// Get keyboard entries.
			entry = process_keyboard::run();
			if (entry == input::QUIT) {
				// Reset all character to nothing before shutdown.
				for (auto& field : id_characters_)
				{
					field.second.set_state(mud::NONE);
				}
				// Remove all character from the game.
				for (auto& field : id_tiles_)
				{
					if (field.second.occupant_type() == mud::CHARACTER)
					{
						field.second.set_occupant_type(mud::NOBODY);
						field.second.set_occupant_id(0);
					}
				}
				running = false;
				continue;
			}
			// Set character moves
			for (auto& id_character : id_characters_)
			{
				if (id_character.second.state() != mud::NONE)
				{
					has_valid_character = true;
					process_character pc(id_character.second);
					current_tile = id_tiles_[id_character.second.tile_id()];
					// Set the character in the gaming field.
					// CHECKME this is potentialy dangerous in multiplayer.
					if (current_tile.occupant_type() == mud::NOBODY)
					{
						current_tile.set_occupant_type(mud::CHARACTER);
						current_tile.set_occupant_id(id_character.first);
					}
					running = pc.run(
						entry, 
						current_tile, 
						around_tiles(current_tile, id_tiles_));
					// Check if character moved update if it did.
					if (current_tile.id() != id_character.second.tile_id())
					{
						current_tile.set_occupant_type(mud::NOBODY);
						current_tile.set_occupant_id(0);
						mud::tile& new_tile = 
							id_tiles_[id_character.second.tile_id()];
						new_tile.set_occupant_type(mud::CHARACTER);
						new_tile.set_occupant_id(id_character.second.id());
						current_tile = new_tile;
					}
				}
			}
			// This is a hack to select a character this should be moved.
			if (!has_valid_character) 
			{
				select_character();
			}
			// Set the enemy moves.
			pe.run();
			// Output the map.
			if (current_tile_id != current_tile.id())
			{
				std::cout << current_tile;
				current_tile_id = current_tile.id();
			}
			auto end_time = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> duration = end_time - start_time;
			if (total_time_ > duration.count())
			{
				int wait_to = 
					static_cast<int>(
					(total_time_ - duration.count()) * 1000.0);
				std::this_thread::sleep_for(
					std::chrono::milliseconds(wait_to));
			}
			else
			{
				std::cerr << "too long..." << std::endl;
			}
		}
	}

	void process_game::select_character()
	{
		std::int64_t free_id = 1;
		for (const auto& field : id_characters_) 
		{
			if (field.second.id() == free_id) free_id++;
			std::cout << field.second;
		}
		std::cout << "enter the name of the character you wanna play" << std::endl;
		std::cout << " > ";
		std::string name;
		std::cin >> name;
		if (name.find("new") != std::string::npos) 
		{
			mud::character character{};
			std::string character_name;
			std::cout << "enter the name of the new character" << std::endl;
			std::cout << " > ";
			std::cin >> character_name;
			character.set_name(character_name);
			character.set_id(free_id);
			character.set_tile_id(1);
			id_characters_.insert({ free_id, character });
		}
		else
		{
			for (auto& field : id_characters_)
			{
				if (field.second.name() == name) 
				{
					field.second.set_state(mud::WALKING);
				}
			}
		}
	}

} // End namespace server.