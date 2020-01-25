#pragma once
#include <map>
#include <cstdint>
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

	class process_enemy 
	{
	public:
		process_enemy(
			std::map<std::int64_t, mud::enemy>& id_enemies,
			std::map<std::int64_t, mud::tile>& id_tiles) :
			id_enemies_(id_enemies),
			id_tiles_(id_tiles) {}
		void run();
	protected:
		void move_to(const mud::enemy& enemy, const mud::tile& tile);
	private:
		std::map<std::int64_t, mud::enemy>& id_enemies_;
		std::map<std::int64_t, mud::tile>& id_tiles_;
	};

} // End namespace server.