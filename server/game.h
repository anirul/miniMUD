#pragma once
#include <cstdint>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <unordered_map>
#ifndef __APPLE__
#include <execution>
#endif
#include "../mud_lib/helper.h"

namespace server {

	class game
	{
	public:
		// Decide if you want binary or json as output and input.
		enum class binary_or_json_t { BINARY, JSON, };
	public:
		// Initiate the game structure.
		game(
			const std::string& player_file = "player.json",
			const std::string& character_file = "character.json",
			const std::string& enemy_file = "enemy.json",
			const std::string& tile_file = "tile.json",
			const std::string& item_file = "item.json",
			binary_or_json_t binary_or_json = binary_or_json_t::JSON);
		// Save the structure at the end of the game.
		virtual ~game();
		// Does return at the end of the game!
		void run_once();
		std::int64_t iteration();
	public:
		const mud::player& get_player(const std::string& name) const;
		const mud::player& get_player(std::int64_t id) const;
		const bool has_player(std::int64_t id) const;
		const mud::tile& get_tile(std::int64_t id) const;
		const bool has_tile(std::int64_t id) const;
		const bool has_player_character(
			const std::int64_t id_player, 
			const std::int64_t id_character) const;
		const mud::character& get_character(const std::int64_t id) const;
		const std::unordered_map<std::int64_t, mud::character>&
			get_characters() const;
		const std::unordered_map<std::int64_t, mud::enemy>& get_enemies() const;
		const std::unordered_map<std::int64_t, mud::item>& get_items() const;
		const std::unordered_map<std::int64_t, mud::tile>& get_tiles() const;
		const std::unordered_map<std::int64_t, mud::player>& 
			get_players() const;
		void activate_character(std::int64_t id);
		void deactivate_character(std::int64_t id);
	public:
		mud::tile& get_tile(std::int64_t id);
		mud::character& get_character(std::int64_t id);
		std::int64_t create_new_character(
			const std::string& name,
			std::int64_t id_playere);
		void add_command(std::int64_t id, mud::play_in::command_enum value);
		// Get an play out of the game for a given character.
		// This remove the struct from the inside map!
		mud::play_out get_play_out(std::int64_t id);
	protected:
		void exit_game();
	protected:
		void execute_characters();
		void execute_enemies();
		void execute_dead();
		void execute_postprocess();
	private:
		const std::string player_file_;
		const std::string character_file_;
		const std::string enemy_file_;
		const std::string tile_file_;
		const std::string item_file_;
		const binary_or_json_t binary_or_json_;
	private:
		// Return the current iteration (used to prevent user from sending
		// multiple or the same message in a single frame).
		std::int64_t iteration_ = 0;
		std::unordered_map<std::string, std::int64_t> name_player_ids_ = {};
		std::unordered_map<std::int64_t, mud::player> id_players_ = {};
		std::unordered_map<std::int64_t, mud::character> id_characters_ = {};
		std::unordered_map<std::int64_t, mud::enemy> id_enemies_ = {};
		std::unordered_map<std::int64_t, mud::tile> id_tiles_ = {};
		std::unordered_map<std::int64_t, mud::item> id_items_ = {};
		std::unordered_map<std::int64_t, mud::play_in::command_enum>
			id_character_command_in_map_ = {};
		std::unordered_map<std::int64_t, mud::play_out::status_enum>
			id_character_command_out_map_ = {};
	};

}