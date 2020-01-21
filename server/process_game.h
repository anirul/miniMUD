#pragma once
#include <cstdint>
#include <string>
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

namespace server {

	class process_game
	{
	public:
		// Initiate the game structure.
		process_game(
			double total_time = 0.1,
			const std::string& player_file = "player.data",
			const std::string& character_file = "character.data",
			const std::string& enemy_file = "enemy.data",
			const std::string& tile_file = "tile.data");
		// Save the structure at the end of the game.
		virtual ~process_game();
		// Does return at the end of the game!
		void run();
	protected:
		void select_character();
	protected:
		double total_time_;
		std::string player_file_;
		std::string character_file_;
		std::string enemy_file_;
		std::string tile_file_;
		std::map<std::int64_t, mud::player> id_players_;
		std::map<std::int64_t, mud::character> id_characters_;
		std::map<std::int64_t, mud::enemy> id_enemies_;
		std::map<std::int64_t, mud::tile> id_tiles_;
	};

}