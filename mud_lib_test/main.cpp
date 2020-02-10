#include <gtest/gtest.h>
#include "helper_test.h"
#include "hash_test.h"

int main(int ac, char** av) {
	testing::InitGoogleTest(&ac, av);
	return RUN_ALL_TESTS();
}