#pragma once

#include <string>
#include <array>

namespace crypto {

	class hash 
	{
	public:
		hash(const std::string& str);
		std::string get_string() const;
		std::array<std::int64_t, 4> get_value() const;
	private:
		std::array<std::int64_t, 4> value_;
	};

} // End namespace crypto.