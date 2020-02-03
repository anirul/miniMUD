#pragma once
#include <map>
#include <cstdint>
#include "../mud_lib/helper.h"

namespace server {

	class process_enemy 
	{
	public:
		void run(
			mud::enemy& enemy,
			std::map<std::int64_t, mud::tile>& id_tiles,
			std::map<std::int64_t, mud::character>& id_characters);
	protected:
		bool move_to(
			mud::enemy& enemy,
			const mud::tile& tile,
			std::map<std::int64_t, mud::tile>& id_tiles);
		bool attack_character(
			const mud::enemy& enemy, 
			mud::character& character);
	};

} // End namespace server.