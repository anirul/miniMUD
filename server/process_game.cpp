#include "process_game.h"

namespace server {

	process_game::process_game(
		const std::chrono::duration<double> total_time,
		const std::string& player_file,
		const std::string& character_file,
		const std::string& enemy_file,
		const std::string& tile_file,
		const std::string& item_file,
		binary_or_json_t binary_or_json) :
		total_time_(total_time),
		player_file_(player_file),
		character_file_(character_file),
		enemy_file_(enemy_file),
		tile_file_(tile_file),
		item_file_(item_file),
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
		{
			mud::item_book book;
			load(item_file_, book);
			for (const mud::item& item : book.items())
			{
				id_items_.insert({ item.id(), item });
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
				mud::tile t = pair.second;
				// Clean the map before saving.
				if (t.occupant_type() != mud::tile::NOBODY)
					t.set_occupant_type(mud::tile::NOBODY);
				if (t.occupant_id() != 0)
					t.set_occupant_id(0);
				*book.add_tiles() = t;
			}
			save(tile_file_, book);
		}
		{
			mud::item_book book;
			for (const auto& pair : id_items_)
			{
				*book.add_items() = pair.second;
			}
			save(item_file_, book);
		}
	}

	void process_game::run()
	{
		bool running = true;
		std::int64_t current_tile_id = 0;
		// This is a hack to select a character.
		if (!has_actif_character()) select_character();
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
			running = execute_characters(entry);
			// Set the enemy moves.
			execute_enemies();
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
			// CHECKME change this when multi players.
			if (execute_dead()) {
				std::cout << "you died!" << std::endl;
				return;
			}
			execute_postprocess();
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
					field.second.set_state(mud::character::WALKING);
				}
			}
		}
	}

	bool process_game::has_actif_character()
	{
		for (auto& id_character : id_characters_)
		{
			if (id_character.second.state() != mud::character::NONE)
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
			field.second.set_state(mud::character::NONE);
		}
		// Remove all character from the game.
		for (auto& field : id_tiles_)
		{
			if (field.second.occupant_type() == mud::tile::CHARACTER)
			{
				field.second.set_occupant_type(mud::tile::NOBODY);
				field.second.set_occupant_id(0);
			}
		}
	}

	bool process_game::execute_characters(const input_t& entry)
	{
		bool running = true;
		for (auto& id_character : id_characters_)
		{
			if (id_character.second.state() != mud::character::NONE)
			{
				process_character pc(id_character.second);
				mud::tile& current_tile = 
					id_tiles_[id_character.second.tile_id()];
				// Set the character in the gaming field.
				// CHECKME this is potentially dangerous in multi player.
				if (current_tile.occupant_type() == mud::tile::NOBODY ||
					current_tile.occupant_id() == 0)
				{
					current_tile.set_occupant_type(mud::tile::CHARACTER);
					current_tile.set_occupant_id(id_character.first);
				}
				running = pc.run(
					entry,
					current_tile,
					around_tiles(current_tile, id_tiles_));
				// Check if character moved update if it did.
				if (current_tile.id() != id_character.second.tile_id())
				{
					current_tile.set_occupant_type(mud::tile::NOBODY);
					current_tile.set_occupant_id(0);
					mud::tile& new_tile =
						id_tiles_[id_character.second.tile_id()];
					new_tile.set_occupant_type(mud::tile::CHARACTER);
					new_tile.set_occupant_id(id_character.second.id());
					current_tile = new_tile;
				}
				id_tiles_.insert({ current_tile.id(), current_tile });
			}
		}
		return running;
	}

	void process_game::execute_enemies()
	{
		for (auto& id_enemy : id_enemies_)
		{
			pe_.run(id_enemy.second, id_tiles_, id_characters_);
		}
	}

	bool process_game::execute_dead()
	{
		for (auto& character : id_characters_)
		{
			for (auto& attr : character.second.attributes())
			{
				if (attr.name() == mud::attribute::LIFE)
				{
					if (attr.score() <= 0)
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	void process_game::execute_postprocess()
	{
		std::mutex mutex_;
		std::vector<std::int64_t> deleted_id;
		std::for_each(
#ifndef __APPLE__
			std::execution::par,
#endif
			id_enemies_.begin(),
			id_enemies_.end(),
			[&mutex_, &deleted_id](const std::pair<std::int64_t, mud::enemy>& p)
		{
			for (auto& attr : p.second.attributes())
			{
				if (attr.name() == mud::attribute::LIFE)
				{
					if (attr.score() <= 0)
					{
						std::lock_guard l(mutex_);
						deleted_id.push_back(p.first);
					}
				}
			}
		});
		// This should not be parallelized.
		std::for_each(
			deleted_id.begin(), 
			deleted_id.end(), 
			[this](const std::int64_t id) 
		{
			id_enemies_.erase(id);
		});
		std::for_each(
#ifndef __APPLE__
			std::execution::par,
#endif
			id_characters_.begin(),
			id_characters_.end(),
			[&mutex_, this](const std::pair<std::int64_t, mud::character>& p)
		{
			for (auto& attr : *id_characters_[p.first].mutable_attributes())
			{
				if (attr.regen() != 0 && attr.score() != attr.score_max())
				{
					std::lock_guard l(mutex_);
					attr.set_score(attr.score() + attr.regen());
				}
			}
		});
	}

} // End namespace server.
