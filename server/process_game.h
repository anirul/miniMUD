#pragma once
#include <cstdint>
#include <string>
#include <chrono>
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#endif
#include "../protobuf_mud_lib/mud_lib.pb.h"
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif
#include "../protobuf_mud_lib/helper.h"
#include "process_keyboard.h"

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
		bool execute_characters(const input_t& entry, mud::tile& current_tile);
	private:
		process_keyboard pk_;
		const std::chrono::duration<double> total_time_;
		const std::string player_file_;
		const std::string character_file_;
		const std::string enemy_file_;
		const std::string tile_file_;
		const binary_or_json_t binary_or_json_;
		std::map<std::int64_t, mud::player> id_players_;
		std::map<std::int64_t, mud::character> id_characters_;
		std::map<std::int64_t, mud::enemy> id_enemies_;
		std::map<std::int64_t, mud::tile> id_tiles_;
	};

}