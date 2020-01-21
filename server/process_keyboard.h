#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../protobuf_mud_lib/helper.h"

namespace server {

	class process_keyboard
	{
	public:
		static input run();
	};

} // End namespace server.