#pragma once
#include <cstdint>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <type_traits>
#ifndef __APPLE__
#include <execution>
#endif
#include "../mud_lib/helper.h"
#include "../mud_lib/keyboard.h"
#include "enemy.h"
#include "character.h"

namespace server {

	class game
	{
	public:
		// Decide if you want binary or json as output and input.
		enum class binary_or_json_t { BINARY, JSON, };
	public:
		// Initiate the game structure.
		game(
			const std::chrono::duration<double> total_time = 
				std::chrono::milliseconds(100),
			const std::string& player_file = "player.json",
			const std::string& character_file = "character.json",
			const std::string& enemy_file = "enemy.json",
			const std::string& tile_file = "tile.json",
			const std::string& item_file = "item.json",
			binary_or_json_t binary_or_json = binary_or_json_t::JSON);
		// Save the structure at the end of the game.
		virtual ~game();
		// Does return at the end of the game!
		void run();
	public:
		std::map<int64_t, mud::player> get_id_players() const 
		{
			return id_players_;
		}
		void set_id_players(
			const std::map<std::int64_t, mud::player>& id_players)
		{
			id_players_ = id_players;
		}
	protected:
		void select_character();
		bool has_actif_character();
		input::input_t execute_keyboard();
		void exit_game();
		bool execute_characters(const input::input_t& entry);
		void execute_enemies();
		bool execute_dead();
		void execute_postprocess();
	private:
		enemy pe_;
		input::keyboard pk_;
		const std::chrono::duration<double> total_time_;
		const std::string player_file_;
		const std::string character_file_;
		const std::string enemy_file_;
		const std::string tile_file_;
		const std::string item_file_;
		const binary_or_json_t binary_or_json_;
		std::map<std::int64_t, mud::player> id_players_;
		std::map<std::int64_t, mud::character> id_characters_;
		std::map<std::int64_t, mud::enemy> id_enemies_;
		std::map<std::int64_t, mud::tile> id_tiles_;
		std::map<std::int64_t, mud::item> id_items_;
	};

}