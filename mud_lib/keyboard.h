#pragma once

#include <mutex>
#include <thread>
#include <array>
#include "../mud_lib/helper.h"
// This has to change to a more portable way.
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace input {

	class keyboard
	{
	public:
#ifdef __APPLE__
        const int VK_RETURN = 0x0d;
        const int VK_DOWN = 0x28;
        const int VK_UP = 0x26;
        const int VK_TAB = 0x09;
        const int VK_LEFT = 0x25;
        const int VK_RIGHT = 0x27;
        const int VK_ESCAPE = 0x1b;
#endif // __APPLE__
		const std::map<mud::play_in::command_enum, int> input_key =
		{
			{ mud::play_in::ATTACK_MELEE,   VK_RETURN },
			{ mud::play_in::BACKWARD,		VK_DOWN },
			{ mud::play_in::FORWARD,		VK_UP },
			{ mud::play_in::INFO,			VK_TAB },
			{ mud::play_in::TURN_LEFT,		VK_LEFT },
			{ mud::play_in::TURN_RIGHT,		VK_RIGHT },
			{ mud::play_in::QUIT,			VK_ESCAPE }
		};
	public:
		void stop();
		void run();
		bool check_released_input(const mud::play_in::command_enum& key);
		mud::play_in::command_enum get_key();
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
		std::map<mud::play_in::command_enum, bool> key_released_ = {};
#ifndef __APPLE__
		std::array<key_state, 256> key_state_inputs_ = {};
		std::array<short, 256> new_key_states_ = { 0 };
		std::array<short, 256> old_key_states_ = { 0 };
#else
        int last_input_ = 0;
#endif
	};

} // End namespace server.
