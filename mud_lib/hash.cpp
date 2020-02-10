#include "hash.h"
#include <cryptopp/sha3.h>
#include <ios>
#include <iosfwd>
#include <algorithm>

namespace crypto {

	hash::hash(const std::string& value) :
		value_{ 0, 0, 0, 0 }
	{
		CryptoPP::SHA3_256 sha3{};
		sha3.Update(
			reinterpret_cast<const CryptoPP::byte*>(
				&*value.begin()),
				std::distance(value.begin(), value.end()) * 
					sizeof(std::string::value_type));
		sha3.TruncatedFinal(
			reinterpret_cast<CryptoPP::byte*>(
				value_.data()),
				value_.size() * sizeof(std::int64_t));
	}

	std::string hash::get_string() const
	{
		std::stringstream ss{};
		bool first = true;
		for (const auto& i : value_)
		{
			if (first)
			{
				first = false;
			}
			else
			{
				ss << ":";
			}
			ss << std::hex << i;
		}
		return ss.str();
	}

	std::array<std::int64_t, 4> hash::get_value() const
	{
		return value_;
	}

} // End namespace crypto.

bool operator<(const crypto::hash& lh, const crypto::hash& rh)
{
	auto lv = lh.get_value();
	auto rv = rh.get_value();
	return std::lexicographical_compare(
		lv.begin(), lv.end(), 
		rv.begin(), rv.end());
}

bool operator==(const crypto::hash& lh, const crypto::hash& rh)
{
	return !(lh < rh || rh < lh);
}

bool operator!=(const crypto::hash& lh, const crypto::hash& rh)
{
	return lh < rh || rh < lh;
}
