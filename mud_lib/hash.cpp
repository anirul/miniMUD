#include "hash.h"
#include <cryptopp/sha3.h>
#include <ios>
#include <iosfwd>
#include <algorithm>
#ifndef __APPLE__
#include <execution>
#endif

namespace crypto {

	hash::hash(const std::string& in) :
		value_{ 0, 0, 0, 0 }
	{
		CryptoPP::SHA3_256 sha3{};
		sha3.Update(
			reinterpret_cast<const CryptoPP::byte*>(in.data()),
			in.size() * sizeof(char));
		sha3.TruncatedFinal(
			reinterpret_cast<CryptoPP::byte*>(value_.data()),
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
	const auto& lv = lh.get_value();
	const auto& rv = rh.get_value();
	// Default lexicographical compare use <.
	return std::lexicographical_compare(
#ifndef __APPLE__
		std::execution::par,
#endif
		lv.begin(), lv.end(), 
		rv.begin(), rv.end());
}

bool operator==(const crypto::hash& lh, const crypto::hash& rh)
{
	const auto& lv = lh.get_value();
	const auto& rv = rh.get_value();
	return std::lexicographical_compare(
#ifndef __APPLE__
		std::execution::par,
#endif
		lv.begin(), lv.end(),
		rv.begin(), rv.end(), 
		[](const std::int64_t r, const std::int64_t l) 
	{
		return r == l;
	});
}

bool operator!=(const crypto::hash& lh, const crypto::hash& rh)
{
	const auto& lv = lh.get_value();
	const auto& rv = rh.get_value();
	return std::lexicographical_compare(
#ifndef __APPLE__
		std::execution::par,
#endif
		lv.begin(), lv.end(),
		rv.begin(), rv.end(),
		[](const std::int64_t r, const std::int64_t l)
	{
		return r != l;
	});
}
