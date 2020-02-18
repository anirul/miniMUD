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
#ifndef __APPLE__
				for (int i = 0; i < 256; ++i)
                {
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
#endif
				{
					std::lock_guard l(mutex_);
					local_running = running_;
#ifdef __APPLE__
                    last_input_ = getchar();
                    auto it = find_if(
                        input_key.begin(),
                        input_key.end(),
                        [this](const auto& p)
                    {
                        return p.second == last_input_;
                    });
                    key_released_[it->first] = true;
#else
					for (const auto& key : input_key)
                    {
						if (key_state_inputs_[key.second].released_)
						{
							key_released_[key.first] = true;
						}
					}
#endif
				}
			} while (local_running);
		});
	}

	bool keyboard::check_released_input(const mud::play_in::command_enum& key)
	{
		std::lock_guard l(mutex_);
		if (key_released_[key]) {
			std::cout << key << std::endl;
			key_released_[key] = false;
			return true;
		}
		return false;
	}

	mud::play_in::command_enum keyboard::get_key()
	{
		for (const auto k : input_key)
		{
			if (check_released_input(k.first)) {
				return k.first;
			}
		}
		return mud::play_in::NO_COMMAND;
	}

	void keyboard::stop()
	{
		{
			std::lock_guard l(mutex_);
			running_ = false;
		}
		backgroud_thread_.join();
	}

} // End namespace server.
