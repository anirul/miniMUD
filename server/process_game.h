#pragma once
#include <cstdint>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <type_traits>
#include "../mud_lib/helper.h"
#include "process_keyboard.h"
#include "process_enemy.h"
#include "process_character.h"

namespace server {

	class process_game
	{
	public:
		// Decide if you want binary or json as output and input.
		enum class binary_or_json_t { BINARY, JSON, };
	public:
		// Initiate the game structure.
		process_game(
			const std::chrono::duration<double> total_time = 
				std::chrono::milliseconds(100),
			const std::string& player_file = "player.json",
			const std::string& character_file = "character.json",
			const std::string& enemy_file = "enemy.json",
			const std::string& tile_file = "tile.json",
			const std::string& item_file = "item.json",
			binary_or_json_t binary_or_json = binary_or_json_t::JSON);
		// Save the structure at the end of the game.
		virtual ~process_game();
		// Does return at the end of the game!
		void run();
	protected:
		void select_character();
		bool has_actif_character();
		input_t execute_keyboard();
		void exit_game();
		bool execute_characters(const input_t& entry);
		void execute_enemies();
	private:
		process_enemy pe_;
		process_keyboard pk_;
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