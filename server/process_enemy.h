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
		void run(
			mud::enemy& enemy,
			std::map<std::int64_t, mud::tile>& id_tiles);
	protected:
		bool move_to(
			mud::enemy& enemy,
			const mud::tile& tile,
			std::map<std::int64_t, mud::tile>& id_tiles);
	};

} // End namespace server.