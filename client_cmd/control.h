#pragma once

#include <memory>
#include "client.h"
#include "../mud_lib/hash.h"
#include "../mud_lib/keyboard.h"

namespace client {

	class control {
	public:
		control(std::shared_ptr<client> c);
		virtual ~control();
		bool get_token();
		bool login();
		bool select_character();
		bool play();
	protected:
		void output(const std::shared_ptr<mud::play_out> out);
	private:
		std::shared_ptr<client> client_ = nullptr;
		std::int64_t id_token_ = 0;
		std::int64_t id_player_ = 0;
		std::int64_t id_character_ = 0;
		std::vector<mud::character> characters_ = {};
		input::keyboard keyboard_ = {};
		std::string hash_tiles_ = "";
		std::string hash_characters_ = "";
		std::string hash_enemies_ = "";
		std::string hash_items_ = "";
	};

} // End namespace client.