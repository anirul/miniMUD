#include "helper_test.h"
#include <utility>

namespace test {

	HelperTest::HelperTest()
	{
		auto default_tile = [](mud::tile& t)
		{
			t.set_type(mud::tile::EMPTY);
			t.set_occupant_id(0);
			t.set_occupant_type(mud::tile::NOBODY);
		};
		auto create_location = [](mud::direction dir, std::int64_t v)
		{
			mud::location l{};
			l.set_id(v);
			*l.mutable_direction() = dir;
			return l;
		};
		{
			mud::tile t{};
			t.set_id(1);
			default_tile(t);
			*t.add_neighbours() = create_location(direction::north, 6);
			*t.add_neighbours() = create_location(direction::west, 4);
			*t.add_neighbours() = create_location(direction::east, 2);
			*t.add_neighbours() = create_location(direction::south, 3);
			basic_tiles_.insert({ 1, t });
		}
		{
			mud::tile t{};
			t.set_id(2);
			default_tile(t);
			*t.add_neighbours() = create_location(direction::west, 1);
			basic_tiles_.insert({ 2, t });
		}
		{
			mud::tile t{};
			t.set_id(3);
			default_tile(t);
			*t.add_neighbours() = create_location(direction::north, 1);
			basic_tiles_.insert({ 3, t });
		}
		{
			mud::tile t{};
			t.set_id(4);
			default_tile(t);
			*t.add_neighbours() = create_location(direction::east, 1);
			*t.add_neighbours() = create_location(direction::north, 5);
			basic_tiles_.insert({ 4, t });
		}
		{
			mud::tile t{};
			t.set_id(5);
			default_tile(t);
			*t.add_neighbours() = create_location(direction::south, 4);
			*t.add_neighbours() = create_location(direction::east, 6);
			basic_tiles_.insert({ 5, t });
		}
		{
			mud::tile t{};
			t.set_id(5);
			default_tile(t);
			*t.add_neighbours() = create_location(direction::west, 5);
			*t.add_neighbours() = create_location(direction::south, 1);
			basic_tiles_.insert({ 6, t });
		}
	}

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

	TEST_F(HelperTest, AroundTilesSizeTest)
	{
		EXPECT_EQ(4, around_tiles(1, basic_tiles_).size());
		EXPECT_EQ(1, around_tiles(2, basic_tiles_).size());
		EXPECT_EQ(1, around_tiles(3, basic_tiles_).size());
		EXPECT_EQ(2, around_tiles(4, basic_tiles_).size());
		EXPECT_EQ(2, around_tiles(5, basic_tiles_).size());
		EXPECT_EQ(2, around_tiles(6, basic_tiles_).size());
	}

	TEST_F(HelperTest, AroundTilesInsideTest)
	{
		{
			std::map<std::int64_t, mud::direction> result =
			{
				{2, direction::east},
				{3, direction::south},
				{4, direction::west},
				{6, direction::north}
			};
			auto around_result = around_tiles(1, basic_tiles_);
			for (std::int64_t i : {2, 3, 4, 6})
			{
				EXPECT_TRUE(result.find(i) != result.end());
				EXPECT_TRUE(result.find(i) != result.end());
				EXPECT_TRUE(result[i] == around_result[i]);
			}
		}
		{
			std::map<std::int64_t, mud::direction> result =
			{
				{1, direction::west}
			};
			auto around_result = around_tiles(2, basic_tiles_);
			for (std::int64_t i : {1})
			{
				EXPECT_TRUE(result.find(i) != result.end());
				EXPECT_TRUE(result.find(i) != result.end());
				EXPECT_TRUE(result[i] == around_result[i]);
			}
		}
		{
			std::map<std::int64_t, mud::direction> result =
			{
				{1, direction::north}
			};
			auto around_result = around_tiles(3, basic_tiles_);
			for (std::int64_t i : {1})
			{
				EXPECT_TRUE(result.find(i) != result.end());
				EXPECT_TRUE(result.find(i) != result.end());
				EXPECT_TRUE(result[i] == around_result[i]);
			}
		}
		{
			std::map<std::int64_t, mud::direction> result =
			{
				{1, direction::east},
				{5, direction::north}
			};
			auto around_result = around_tiles(4, basic_tiles_);
			for (std::int64_t i : {1, 5})
			{
				EXPECT_TRUE(result.find(i) != result.end());
				EXPECT_TRUE(result.find(i) != result.end());
				EXPECT_TRUE(result[i] == around_result[i]);
			}
		}
		{
			std::map<std::int64_t, mud::direction> result =
			{
				{4, direction::south},
				{6, direction::east}
			};
			auto around_result = around_tiles(5, basic_tiles_);
			for (std::int64_t i : {4, 6})
			{
				EXPECT_TRUE(result.find(i) != result.end());
				EXPECT_TRUE(result.find(i) != result.end());
				EXPECT_TRUE(result[i] == around_result[i]);
			}
		}
		{
			std::map<std::int64_t, mud::direction> result =
			{
				{1, direction::south},
				{5, direction::west}
			};
			auto around_result = around_tiles(6, basic_tiles_);
			for (std::int64_t i : {1, 5})
			{
				EXPECT_TRUE(result.find(i) != result.end());
				EXPECT_TRUE(result.find(i) != result.end());
				EXPECT_TRUE(result[i] == around_result[i]);
			}
		}
	}

	TEST_F(HelperTest, SeeAroundTilesSizeTest)
	{
		EXPECT_EQ(5, see_around_tiles(1, basic_tiles_).size());
		EXPECT_EQ(4, see_around_tiles(2, basic_tiles_).size());
		EXPECT_EQ(4, see_around_tiles(3, basic_tiles_).size());
		EXPECT_EQ(5, see_around_tiles(4, basic_tiles_).size());
		EXPECT_EQ(3, see_around_tiles(5, basic_tiles_).size());
		EXPECT_EQ(5, see_around_tiles(6, basic_tiles_).size());
	}

	TEST_F(HelperTest, SeeAroundTilesInsideTest)
	{
		{
			auto result = see_around_tiles(1, basic_tiles_);
			for (std::int64_t i : {2, 3, 4, 5, 6})
			{
				EXPECT_TRUE(result.find(i) != result.end());
			}
		}
		{
			auto result = see_around_tiles(2, basic_tiles_);
			for (std::int64_t i : {1, 3, 4, 6})
			{
				EXPECT_TRUE(result.find(i) != result.end());
			}
		}
		{
			auto result = see_around_tiles(3, basic_tiles_);
			for (std::int64_t i : {1, 2, 4, 6})
			{
				EXPECT_TRUE(result.find(i) != result.end());
			}
		}
		{
			auto result = see_around_tiles(4, basic_tiles_);
			for (std::int64_t i : {1, 2, 3, 5, 6})
			{
				EXPECT_TRUE(result.find(i) != result.end());
			}
		}
		{
			auto result = see_around_tiles(5, basic_tiles_);
			for (std::int64_t i : {1, 4, 6})
			{
				EXPECT_TRUE(result.find(i) != result.end());
			}
		}
		{
			auto result = see_around_tiles(6, basic_tiles_);
			for (std::int64_t i : {1, 2, 3, 4, 5})
			{
				EXPECT_TRUE(result.find(i) != result.end());
			}
		}
	}

} // End namespace test.
