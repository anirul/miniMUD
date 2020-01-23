#pragma once

#include <mutex>
#include <thread>
#include <array>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../protobuf_mud_lib/helper.h"

namespace server {

	class process_keyboard
	{
	public:
		const std::map<input, int> input_key =
		{
			{ input::ATTACK,   VK_RETURN },
			{ input::BACKWARD, VK_DOWN },
			{ input::FORWARD,  VK_UP },
			{ input::INFO,     VK_TAB },
			{ input::LEFT,     VK_LEFT },
			{ input::RIGHT,    VK_RIGHT },
			{ input::PRINT,    VK_SNAPSHOT },
			{ input::QUIT,     VK_ESCAPE }
		};
	public:
		void stop();
		void run();
		bool check_released_input(input key);
	protected:
		struct key_state
		{
			bool pressed_ = false;
			bool released_ = false;
			bool held_ = false;
		};
		std::thread backgroud_thread_;
		std::mutex mutex_;
		bool running_ = false;
		std::map<input, bool> key_released_ = {};
		std::array<key_state, 256> key_state_inputs_ = {};
		std::array<short, 256> new_key_states_ = { 0 };
		std::array<short, 256> old_key_states_ = { 0 };
	};

} // End namespace server.