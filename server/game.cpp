#include "game.h"
#include "character.h"
#include "enemy.h"

namespace server {

	game::game(
		const std::string& player_file,
		const std::string& character_file,
		const std::string& enemy_file,
		const std::string& tile_file,
		const std::string& item_file,
		binary_or_json_t binary_or_json) :
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
				name_player_ids_.insert({ player.name(), player.id() });
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

	game::~game()
	{
		exit_game();
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

	void game::run_once()
	{
		// Set character moves
		execute_characters();
		// Set the enemy moves.
		execute_enemies();
		// Remove the dead.
		execute_dead();
		// Cleanup.
		execute_postprocess();
		iteration_++;
	}

	std::int64_t game::iteration()
	{
		return iteration_;
	}

	const mud::player& game::get_player(const std::string& name) const
	{
		auto it = name_player_ids_.find(name);
		if (it == name_player_ids_.end())
		{
			throw std::runtime_error("no player with name :" + name);
		}
		return id_players_.at(it->second);
	}

	const mud::player& game::get_player(std::int64_t id) const
	{
		return id_players_.at(id);
	}

	const bool game::has_player(std::int64_t id) const
	{
		if (id == 0) return false;
		const auto& it = id_players_.find(id);
		if (it == id_players_.end())
		{
			return false;
		}
		return true;
	}

	const mud::tile& game::get_tile(std::int64_t id) const
	{
		return id_tiles_.at(id);
	}

	mud::tile& game::get_tile(std::int64_t id)
	{
		return id_tiles_[id];
	}

	const bool game::has_tile(std::int64_t id) const
	{
		if (id == 0) return false;
		const auto& it = id_tiles_.find(id);
		if (it == id_tiles_.end()) 
		{
			return false;
		}
		return true;
	}

	const bool game::has_player_character(
		const std::int64_t id_player, 
		const std::int64_t id_character) const
	{
		if (id_player == 0) return false;
		if (id_character == 0) return false;
		const auto player = id_players_.at(id_player);
		for (const auto id : player.id_characters())
		{
			if (id == id_character) return true;
		}
		return false;
	}

	const mud::character& game::get_character(const std::int64_t id) const
	{
		return id_characters_.at(id);
	}

	mud::character& game::get_character(std::int64_t id)
	{
		return id_characters_[id];
	}

	const std::unordered_map<std::int64_t, mud::character>&
		game::get_characters() const
	{
		return id_characters_;
	}

	const std::unordered_map<std::int64_t, mud::enemy>& 
		game::get_enemies() const
	{
		return id_enemies_;
	}

	const std::unordered_map<std::int64_t, mud::item>& game::get_items() const
	{
		return id_items_;
	}

	const std::unordered_map<std::int64_t, mud::tile>& game::get_tiles() const
	{
		return id_tiles_;
	}

	const std::unordered_map<std::int64_t, mud::player>& 
		game::get_players() const
	{
		return id_players_;
	}

	void game::activate_character(std::int64_t id)
	{
		auto it = id_characters_.find(id);
		if (it != id_characters_.end())
		{
			it->second.set_state(mud::character::WALKING);
		}
	}

	void game::deactivate_character(std::int64_t id)
	{
		auto it = id_characters_.find(id);
		if (it != id_characters_.end())
		{
			it->second.set_state(mud::character::NONE);
		}
	}

	std::int64_t game::create_new_character(
		const std::string& name, 
		std::int64_t id_player)
	{
		static std::int64_t new_id = 1;
		while (id_characters_.find(new_id) == id_characters_.end())
		{
			new_id++;
		}
		auto new_character = id_characters_[0];
		new_character.set_name(name);
		new_character.set_id(new_id);
		id_characters_[new_id] = new_character;
		id_players_[id_player].add_id_characters(new_id);
		return new_id;
	}

	void game::add_command(std::int64_t id, mud::play_in::command_enum value)
	{
		id_character_command_in_map_.insert({ id, value });
	}

	mud::play_out game::get_play_out(std::int64_t id)
	{
		mud::play_out out{};
		std::pair<std::int64_t, mud::play_out::status_enum> value;
		// Get the value of return for this player.
		{
			const auto it = id_character_command_out_map_.find(id);
			if (it == id_character_command_out_map_.end())
			{
				value.first = id;
				value.second = mud::play_out::SUCCESS;
			}
			else
			{
				value = *it;
				id_character_command_out_map_.erase(it);
			}
		}
		const auto& it_id_me = id_characters_.find(value.first);
		if (it_id_me == id_characters_.end()) return {};
		const auto& see_tiles = see_around_tiles(
			it_id_me->second.tile_id(), 
			get_tiles());
		std::set<std::int64_t> character_set = { value.first };
		std::set<std::int64_t> enemy_set = {};
		std::set<std::int64_t> tile_set = { it_id_me->second.tile_id() };
		std::set<std::int64_t> item_set = {};
		out.set_id_character(value.first);
		out.set_status(value.second);
		for (const auto& field : see_tiles)
		{
			if (field.first == 0) continue;
			const auto it = id_tiles_.find(field.first);
			if (it == id_tiles_.end()) continue;
			if (it->second.id() == 0) continue;
			tile_set.insert(field.first);
			if (it->second.occupant_type() == mud::tile::CHARACTER)
			{
				const auto it_character =
					id_characters_.find(it->second.occupant_id());
				if (it_character == id_characters_.end()) continue;
				if (it_character->first != 0) {
					character_set.insert(it->second.occupant_id());
					for (const auto& id_item :
						it_character->second.equiped_item_ids())
					{
						item_set.insert(id_item);
					}
					for (const auto& id_item :
						it_character->second.stored_item_ids())
					{
						item_set.insert(id_item);
					}
				}
			}
			if (it->second.occupant_type() == mud::tile::ENEMY)
			{
				const auto it_enemy =
					id_enemies_.find(it->second.occupant_id());
				if (it_enemy == id_enemies_.end()) continue;
				if (it_enemy->first != 0)
				{
					enemy_set.insert(it->second.occupant_id());
					*out.add_enemies() = it_enemy->second;
					for (const auto& id_item :
						it_enemy->second.droppable_item_ids())
					{
						item_set.insert(id_item);
					}
				}
			}
		}
		for (const auto id : character_set)
		{
			*out.add_characters() = id_characters_[id];
		}
		for (const auto id : tile_set)
		{
			*out.add_tiles() = id_tiles_[id];
		}
		for (const auto id : enemy_set)
		{
			*out.add_enemies() = id_enemies_[id];
		}
		for (const auto id : item_set)
		{
			*out.add_items() = id_items_[id];
		}
		out.set_status(mud::play_out::SUCCESS);
		return out;
	}

	void game::exit_game()
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

	void game::execute_characters()
	{
		character pc{ *this };
		id_character_command_out_map_ = pc.run(
			id_character_command_in_map_,
			id_characters_);
		id_character_command_in_map_.clear();
	}

	void game::execute_enemies()
	{
		enemy pe{ *this };
		pe.run(id_enemies_);
	}

	void game::execute_dead()
	{
		for (auto& character : id_characters_)
		{
			for (auto& attr : character.second.attributes())
			{
				if (attr.name() == mud::attribute::LIFE)
				{
					if (attr.score() <= 0)
					{
						id_character_command_out_map_[character.first] =
							mud::play_out::DEAD;
						break;
					}
				}
			}
		}
	}

	void game::execute_postprocess()
	{
		std::mutex local_mutex_;
		std::vector<std::int64_t> deleted_id;
		std::for_each(
#ifndef __APPLE__
			std::execution::par,
#endif
			id_enemies_.begin(),
			id_enemies_.end(),
			[&local_mutex_, &deleted_id](
				const std::pair<std::int64_t, mud::enemy>& p)
		{
			for (auto& attr : p.second.attributes())
			{
				if (attr.name() == mud::attribute::LIFE)
				{
					if (attr.score() <= 0)
					{
						std::scoped_lock(local_mutex_);
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
			[&local_mutex_, this](
				const std::pair<std::int64_t, mud::character>& p)
		{
			for (auto& attr : *id_characters_[p.first].mutable_attributes())
			{
				if (attr.regen() != 0 && attr.score() != attr.score_max())
				{
					std::scoped_lock(local_mutex_);
					attr.set_score(attr.score() + attr.regen());
				}
			}
		});
	}

} // End namespace server.
