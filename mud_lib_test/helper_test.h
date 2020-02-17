#pragma once

#include <gtest/gtest.h>
#include "../mud_lib/helper.h"

namespace test {

	class HelperTest : public testing::Test {
	public:
		HelperTest();
	protected:
		std::unordered_map<std::int64_t, mud::tile> basic_tiles_ = {};
	};

} // End namespace test.
