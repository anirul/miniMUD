#include "keyboard.h"

namespace input {

	void keyboard::run()
	{
		if (running_) return;
		running_ = true;
		backgroud_thread_ = std::thread([this]
		{
			bool local_running = true;
			do {
				for (int i = 0; i < 256; ++i) {
					new_key_states_[i] = GetAsyncKeyState(i);
					key_state_inputs_[i].pressed_ = false;
					key_state_inputs_[i].released_ = false;
					if (new_key_states_[i] != old_key_states_[i])
					{
						if (new_key_states_[i] & 0x8000)
						{
							key_state_inputs_[i].pressed_ = 
								key_state_inputs_[i].held_;
							key_state_inputs_[i].held_ = true;
						}
						else
						{
							key_state_inputs_[i].released_ = true;
							key_state_inputs_[i].held_ = false;
						}
					}
					old_key_states_[i] = new_key_states_[i];
				}
				{
					std::lock_guard l(mutex_);
					local_running = running_;
					for (const auto& key : input_key) {
						if (key_state_inputs_[key.second].released_)
						{
							key_released_[key.first] = true;
						}
					}
				}
			} while (local_running);
		});
	}

	bool keyboard::check_released_input(const input_t& key)
	{
		std::lock_guard l(mutex_);
		if (key_released_[key]) {
			std::cout << key << std::endl;
			key_released_[key] = false;
			return true;
		}
		return false;
	}

	void keyboard::stop() {
		{
			std::lock_guard l(mutex_);
			running_ = false;
		}
		backgroud_thread_.join();
	}

} // End namespace server.

std::ostream& operator<< (std::ostream& os, const input::input_t& key)
{
	switch (key)
	{
	case input::input_t::ATTACK:
		os << "ATTACK";
		break;
	case input::input_t::BACKWARD:
		os << "BACKWARD";
		break;
	case input::input_t::FORWARD:
		os << "FORWARD";
		break;
	case input::input_t::INFO:
		os << "INFO";
		break;
	case input::input_t::LEFT:
		os << "LEFT";
		break;
	case input::input_t::NONE:
		os << "NONE";
		break;
	case input::input_t::PRINT:
		os << "PRINT";
		break;
	case input::input_t::QUIT:
		os << "QUIT";
		break;
	case input::input_t::RIGHT:
		os << "RIGHT";
		break;
	}
	return os;
}