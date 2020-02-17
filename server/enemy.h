#pragma once
#include <map>
#include <cstdint>
#include "../mud_lib/helper.h"
#include "game.h"

namespace server {

	class enemy 
	{
	public:
		enemy(game& g) : game_(g) {}
		void run(std::unordered_map<std::int64_t, mud::enemy>& id_enemies);
	protected:
		bool move_to(std::int64_t from, std::int64_t to, mud::enemy& me);
		bool attack_character(mud::character& character, const mud::enemy& me);
	private:
		game& game_;
	};

} // End namespace server.