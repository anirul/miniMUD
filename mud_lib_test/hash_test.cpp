#include "hash_test.h"

namespace test {

	TEST_F(HashTest, GetStringTest)
	{
		crypto::hash h1{ "test hash 1" };
		std::string s1 = h1.get_string();
		auto pos = s1.find_first_not_of("1234567890abcdef:");
		EXPECT_TRUE(pos == std::string::npos);
	}

	TEST_F(HashTest, ValueTest)
	{
		crypto::hash h1{ "test hash 2" };
		auto v1 = h1.get_value();
		EXPECT_EQ(v1[0], 0xf597c755d16158c1);
		EXPECT_EQ(v1[1], 0x8b354898418ae47b);
		EXPECT_EQ(v1[2], 0x50359a886721fe2b);
		EXPECT_EQ(v1[3], 0xa4fd3b04b042e86c);
	}

	TEST_F(HashTest, CompareStringTest)
	{
		crypto::hash h1{ "test hash 3" };
		crypto::hash h2{ "test hash 3" };
		crypto::hash h3{ "another hash" };
		std::string s1 = h1.get_string();
		std::string s2 = h2.get_string();
		std::string s3 = h3.get_string();
		EXPECT_EQ(s1, s2);
		EXPECT_NE(s1, s3);
	}

	TEST_F(HashTest, CompareTest) 
	{
		crypto::hash h1{ "test hash 4" };
		crypto::hash h2{ "test hash 4" };
		crypto::hash h3{ "another hash" };
		EXPECT_TRUE(h1 == h2);
		EXPECT_FALSE(h1 != h2);
		EXPECT_FALSE(h1 < h2);
		EXPECT_FALSE(h2 < h1);
		EXPECT_TRUE(h1 < h3 || h3 < h1);
		EXPECT_FALSE(h1 == h3);
		EXPECT_TRUE(h1 != h3);
	}

} // End namespace test.