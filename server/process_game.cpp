#include "process_game.h"

#include <type_traits>
#include "../protobuf_mud_lib/helper.h"
#include "process_character.h"
#include "process_enemy.h"
#include "process_keyboard.h"
#include "process_enemy.h"

namespace server {

	process_game::process_game(
		const std::chrono::duration<double> total_time,
		const std::string& player_file,
		const std::string& character_file,
		const std::string& enemy_file,
		const std::string& tile_file,
		binary_or_json_t binary_or_json) :
		total_time_(total_time),
		player_file_(player_file),
		character_file_(character_file),
		enemy_file_(enemy_file),
		tile_file_(tile_file),
		binary_or_json_(binary_or_json)
	{
		auto load = [this](const std::string& file, auto& book)
		{
			if (binary_or_json_ == binary_or_json_t::BINARY)
			{
				book = read_file<std::decay_t<decltype(book)>>(file);
			}
			if (binary_or_json_ == binary_or_json_t::JSON)
			{
				book = read_json<std::decay_t<decltype(book)>>(file);
			}
		};
		{
			mud::player_book book;
			load(player_file_, book);
			for (const mud::player& player : book.players()) 
			{
				id_players_.insert({ player.id(), player });
			}
		}
		{
			mud::character_book book;
			load(character_file_, book);
			for (const mud::character& character : book.characters()) 
			{
				id_characters_.insert({ character.id(), character });
			}
		}
		{
			mud::enemy_book book;
			load(enemy_file_, book);
			for (const mud::enemy& enemy : book.enemies()) 
			{
				id_enemies_.insert({ enemy.id(), enemy });
			}
		}
		{
			mud::tile_book book;
			load(tile_file_, book);
			for (const mud::tile& tile : book.tiles())
			{
				id_tiles_.insert({ tile.id(), tile });
			}
		}
	}

	process_game::~process_game()
	{
		auto save = [this](const std::string& file, const auto& book)
		{
			if (binary_or_json_ == binary_or_json_t::BINARY)
			{
				write_file(file, book);
			}
			if (binary_or_json_ == binary_or_json_t::JSON)
			{
				write_json(file, book);
			}
		};
		{
			mud::player_book book;
			for (const auto& pair : id_players_)
			{
				*book.add_players() = pair.second;
			}
			save(player_file_, book);
		}
		{
			mud::character_book book;
			for (const auto& pair : id_characters_)
			{
				*book.add_characters() = pair.second;
			}
			save(character_file_, book);
		}
		{
			mud::enemy_book book;
			for (const auto& pair : id_enemies_)
			{
				*book.add_enemies() = pair.second;
			}
			save(enemy_file_, book);
		}
		{
			mud::tile_book book;
			for (const auto& pair : id_tiles_)
			{
				*book.add_tiles() = pair.second;
			}
			save(tile_file_, book);
		}
	}

	void process_game::run()
	{
		bool running = true;
		std::int64_t current_tile_id = 0;
		mud::tile& current_tile = mud::tile{};
		// This is a hack to select a character.
		if (!has_actif_character()) select_character();
		process_enemy pe(id_enemies_, id_tiles_);
		pk_.run();
		while (running) 
		{
			auto start_time = std::chrono::high_resolution_clock::now();
			// Check keyboard.
			input_t entry = execute_keyboard();
			// Special case of quit.
			if (entry == input_t::QUIT)
			{
				exit_game();
				running = false;
				continue;
			}
			// Set character moves
			running = execute_characters(entry, current_tile);
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
			if (total_time_ > duration)
			{
				std::this_thread::sleep_for(total_time_ - duration);
			}
			else
			{
				std::cerr << "too long..." << std::endl;
			}
		}
		pk_.stop();
	}

	void process_game::select_character()
	{
		std::int64_t free_id = 1;
		for (const auto& field : id_characters_) 
		{
			if (field.second.id() == free_id) free_id++;
			std::cout << field.second;
		}
		std::cout 
			<< "enter the name of the character you wanna play" 
			<< std::endl;
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

	bool process_game::has_actif_character()
	{
		for (auto& id_character : id_characters_)
		{
			if (id_character.second.state() != mud::NONE)
			{
				return true;
			}
		}
		return false;
	}

	input_t process_game::execute_keyboard()
	{
		input_t entry = input_t::NONE;
		// Get keyboard entries.
		for (const auto& field : pk_.input_key)
		{
			if (pk_.check_released_input(field.first))
			{
				return field.first;
			}
		}
		return entry;
	}

	void process_game::exit_game()
	{
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
	}

	bool process_game::execute_characters(
		const input_t& entry, 
		mud::tile& current_tile)
	{
		bool running = true;
		for (auto& id_character : id_characters_)
		{
			if (id_character.second.state() != mud::NONE)
			{
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
		return running;
	}

} // End namespace server.