#include "helper_test.h"
#include <utility>

namespace test {

	TEST_F(HelperTest, LessDirectionTest)
	{
		std::less<mud::direction> less_direction{};
		EXPECT_FALSE(less_direction(direction::north, direction::north));
	}

	TEST_F(HelperTest, ComparaisonDirectionTest)
	{
		mud::direction dir{};
		{
			dir.set_value(mud::direction::NORTH);
			EXPECT_TRUE(direction::north == dir);
			EXPECT_FALSE(direction::north != dir);
		}
		{
			dir.set_value(mud::direction::SOUTH);
			EXPECT_TRUE(direction::south == dir);
			EXPECT_FALSE(direction::south != dir);
		}
		{
			dir.set_value(mud::direction::EAST);
			EXPECT_TRUE(direction::east == dir);
			EXPECT_FALSE(direction::east != dir);
		}
		{
			dir.set_value(mud::direction::WEST);
			EXPECT_TRUE(direction::west == dir);
			EXPECT_FALSE(direction::west != dir);
		}
	}

	TEST_F(HelperTest, GetInverseDirectionTest)
	{
		mud::direction dir{};
		for (const auto& value : {
			mud::direction::NORTH,
			mud::direction::SOUTH,
			mud::direction::EAST,
			mud::direction::WEST })
		{
			dir.set_value(value);
			EXPECT_FALSE(dir == get_invert_direction(dir));
			EXPECT_TRUE(dir != get_invert_direction(dir));
			EXPECT_TRUE(dir == get_invert_direction(get_invert_direction(dir)));
		}
	}

	TEST_F(HelperTest, GetLeftDirectionTest)
	{
		mud::direction dir{};
		for (const auto& value : {
			mud::direction::NORTH,
			mud::direction::SOUTH,
			mud::direction::EAST,
			mud::direction::WEST })
		{
			dir.set_value(value);
			EXPECT_FALSE(dir == get_left_direction(dir));
			EXPECT_TRUE(dir != get_left_direction(dir));
			EXPECT_TRUE(dir == get_left_direction(
				get_left_direction(
					get_left_direction(
						get_left_direction(dir)))));
		}
	}

	TEST_F(HelperTest, GetRightDirectionTest)
	{
		mud::direction dir{};
		for (const auto& value : {
			mud::direction::NORTH,
			mud::direction::SOUTH,
			mud::direction::EAST,
			mud::direction::WEST })
		{
			dir.set_value(value);
			EXPECT_FALSE(dir == get_right_direction(dir));
			EXPECT_TRUE(dir != get_right_direction(dir));
			EXPECT_TRUE(dir == get_right_direction(
				get_right_direction(
					get_right_direction(
						get_right_direction(dir)))));
		}
	}

	TEST_F(HelperTest, GetRandomDirectionTest)
	{
		const long large = 10000;
		std::map<mud::direction, long> map{
			{ direction::north, 0 },
			{ direction::south, 0 },
			{ direction::east, 0 },
			{ direction::west, 0 }
		};
		for (long i = 0; i < large; ++i)
		{
			map[get_random_direction()]++;
		}
		// This should not happens but who knows?
		for (const auto& field : map) {
			EXPECT_TRUE(field.second > 0);
			EXPECT_TRUE(field.second > (large / 5));
		}
	}

	TEST_F(HelperTest, IsTileEmptyTest)
	{
		mud::tile t{};
		{
			t.set_occupant_id(0);
			t.set_occupant_type(mud::tile::NOBODY);
			EXPECT_TRUE(is_tile_empty(t));
		}
		{
			t.set_occupant_id(42);
			t.set_occupant_type(mud::tile::NOBODY);
			EXPECT_FALSE(is_tile_empty(t));
		}
		{
			t.set_occupant_id(0);
			t.set_occupant_type(mud::tile::ENEMY);
			EXPECT_FALSE(is_tile_empty(t));
		}
		{
			t.set_occupant_id(42);
			t.set_occupant_type(mud::tile::ENEMY);
			EXPECT_FALSE(is_tile_empty(t));
		}
		{
			t.set_occupant_id(0);
			t.set_occupant_type(mud::tile::CHARACTER);
			EXPECT_FALSE(is_tile_empty(t));
		}
		{
			t.set_occupant_id(42);
			t.set_occupant_type(mud::tile::CHARACTER);
			EXPECT_FALSE(is_tile_empty(t));
		}
	}

	TEST_F(HelperTest, IsTileEmptyOrEnemyTest)
	{
		mud::tile t{};
		{
			t.set_occupant_id(0);
			t.set_occupant_type(mud::tile::NOBODY);
			EXPECT_TRUE(is_tile_empty_or_enemy(t));
		}
		{
			t.set_occupant_id(42);
			t.set_occupant_type(mud::tile::NOBODY);
			EXPECT_FALSE(is_tile_empty_or_enemy(t));
		}
		{
			t.set_occupant_id(0);
			t.set_occupant_type(mud::tile::ENEMY);
			EXPECT_FALSE(is_tile_empty_or_enemy(t));
		}
		{
			t.set_occupant_id(42);
			t.set_occupant_type(mud::tile::ENEMY);
			EXPECT_TRUE(is_tile_empty_or_enemy(t));
		}
		{
			t.set_occupant_id(0);
			t.set_occupant_type(mud::tile::CHARACTER);
			EXPECT_FALSE(is_tile_empty_or_enemy(t));
		}
		{
			t.set_occupant_id(42);
			t.set_occupant_type(mud::tile::CHARACTER);
			EXPECT_FALSE(is_tile_empty_or_enemy(t));
		}
	}

	TEST_F(HelperTest, IsTileEmptyOrCharacterTest)
	{
		mud::tile t{};
		{
			t.set_occupant_id(0);
			t.set_occupant_type(mud::tile::NOBODY);
			EXPECT_TRUE(is_tile_empty_or_character(t));
		}
		{
			t.set_occupant_id(42);
			t.set_occupant_type(mud::tile::NOBODY);
			EXPECT_FALSE(is_tile_empty_or_character(t));
		}
		{
			t.set_occupant_id(0);
			t.set_occupant_type(mud::tile::ENEMY);
			EXPECT_FALSE(is_tile_empty_or_character(t));
		}
		{
			t.set_occupant_id(42);
			t.set_occupant_type(mud::tile::ENEMY);
			EXPECT_FALSE(is_tile_empty_or_character(t));
		}
		{
			t.set_occupant_id(0);
			t.set_occupant_type(mud::tile::CHARACTER);
			EXPECT_FALSE(is_tile_empty_or_character(t));
		}
		{
			t.set_occupant_id(42);
			t.set_occupant_type(mud::tile::CHARACTER);
			EXPECT_TRUE(is_tile_empty_or_character(t));
		}

	}

} // End namespace test.
