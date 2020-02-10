#include "helper.h"
#include <stack>
#include <array>
#include <algorithm>
#include <random>

bool operator==(const mud::direction& l, const mud::direction& r)
{
	return l.value() == r.value();
}

bool operator!=(const mud::direction& l, const mud::direction& r)
{
	return l.value() != r.value();
}

std::ostream& operator<< (std::ostream& os, const mud::player& player)
{
	os << "name     : " << player.name() << std::endl;
	os << "password : " << player.password_hash() << std::endl;
	os << "id       : " << player.id() << std::endl;
	os << "characters(" << player.id_characters().size() << ")" << std::endl;
	for (const auto& id : player.id_characters())
	{
		os << "\t" << id << std::endl;
	}
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::character& character)
{
	os << "name     : " << character.name() << std::endl;
	os << "id       : " << character.id() << std::endl;
	os << "tile id  : " << character.tile_id() << std::endl;
	os << "facing   : " << character.facing() << std::endl;
	os << "state    : " << character.state() << std::endl;
	for (const auto& attr : character.attributes())
	{
		os << "\tname      : " << attr.name() << std::endl;
		os << "\tscore     : " << attr.score() 
			<< " [" << attr.score_max() << "]" << std::endl;
		os << "\tregen     : " << attr.regen() << std::endl;
	}
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::tile& tile)
{
	os << "mood     : " << tile.mood() << std::endl;
	os << "type     : " << tile.type() << std::endl;
	os << "id       : " << tile.id() << std::endl;
	os << "occ type : " << tile.occupant_type() << std::endl;
	os << "occ id   : " << tile.occupant_id() << std::endl;
	os << "neighbours(" << tile.neighbours().size() << ")" << std::endl;
	for (const auto& field : tile.neighbours()) 
	{
		os << "\tlocation direction : " << field.direction() << std::endl;
		os << "\tlocation id        : " << field.id() << std::endl;
	}
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::direction& direction)
{
	switch (direction.value()) 
	{
	case mud::direction::NORTH:
		os << "NORTH";
		break;
	case mud::direction::SOUTH:
		os << "SOUTH";
		break;
	case mud::direction::WEST:
		os << "WEST";
		break;
	case mud::direction::EAST:
		os << "EAST";
		break;
	}
	return os;
}

std::ostream& operator<< (
	std::ostream& os, 
	const mud::character::character_state_enum& state)
{
	switch (state) 
	{
	case mud::character::NONE:
		os << "NONE";
		break;
	case mud::character::WALKING:
		os << "WALKING";
		break;
	case mud::character::COMBAT:
		os << "COMBAT";
		break;
	}
	return os;
}

std::ostream& operator<< (
	std::ostream& os, 
	const mud::tile::resident_type_enum& resident)
{
	switch (resident)
	{
	case mud::tile::NOBODY:
		os << "NOBODY";
		break;
	case mud::tile::CHARACTER:
		os << "CHARACTER";
		break;
	case mud::tile::ENEMY:
		os << "ENEMY";
		break;
	}
	return os;
}

std::ostream& operator<< (
	std::ostream& os, 
	const mud::tile::tile_type_enum& tile)
{
	switch (tile)
	{
	case mud::tile::EMPTY:
		os << "EMPTY";
		break;
	case mud::tile::WALL:
		os << "WALL";
		break;
	case mud::tile::TREE:
		os << "TREE";
		break;
	case mud::tile::PORTAL:
		os << "PORTAL";
		break;
	}
	return os;
}

std::ostream& operator<< (
	std::ostream& os,
	const mud::attribute::attribute_enum& name)
{
	switch (name)
	{
	case mud::attribute::LIFE:
		os << "LIFE";
		break;
	case mud::attribute::STRENGTH:
		os << "STRENGTH";
		break;
	case mud::attribute::AGILITY:
		os << "AGILITY";
		break;
	case mud::attribute::INTELLIGENCE:
		os << "INTELLIGENCE";
		break;
	}
	return os;
}

mud::direction get_invert_direction(const mud::direction& dir)
{
	mud::direction out{};
	switch (dir.value())
	{
	case mud::direction::NORTH:
		out.set_value(mud::direction::SOUTH);
		break;
	case mud::direction::SOUTH:
		out.set_value(mud::direction::NORTH);
		break;
	case mud::direction::EAST:
		out.set_value(mud::direction::WEST);
		break;
	case mud::direction::WEST:
		out.set_value(mud::direction::EAST);
		break;
	default:
		out = get_random_direction();
		break;
	}
	return out;
}

mud::direction get_left_direction(const mud::direction& dir)
{
	mud::direction out{};
	switch (dir.value())
	{
	case mud::direction::NORTH:
		out.set_value(mud::direction::WEST);
		break;
	case mud::direction::SOUTH:
		out.set_value(mud::direction::EAST);
		break;
	case mud::direction::EAST:
		out.set_value(mud::direction::NORTH);
		break;
	case mud::direction::WEST:
		out.set_value(mud::direction::SOUTH);
		break;
	default:
		out = get_random_direction();
		break;
	}
	return out;
}

mud::direction get_right_direction(const mud::direction& dir)
{
	mud::direction out{};
	out.set_value(mud::direction::NORTH);
	switch (dir.value())
	{
	case mud::direction::NORTH:
		out.set_value(mud::direction::EAST);
		break;
	case mud::direction::SOUTH:
		out.set_value(mud::direction::WEST);
		break;
	case mud::direction::EAST:
		out.set_value(mud::direction::SOUTH);
		break;
	case mud::direction::WEST:
		out.set_value(mud::direction::NORTH);
		break;
	default:
		out = get_random_direction();
		break;
	}
	return out;
}

std::ostream& operator<< (
	std::ostream& os, 
	const std::vector<mud::direction>& stack)
{
	for (const auto& field : stack)
	{
		os << field << std::endl;
	}
	return os;
}

mud::direction get_random_direction() {
	mud::direction out{};
	static std::random_device rd{};
	static std::mt19937 gen(rd());
	static std::vector<mud::direction::direction_enum> directions = {
		mud::direction::NORTH, 
		mud::direction::SOUTH, 
		mud::direction::WEST, 
		mud::direction::EAST };
	static std::uniform_int_distribution<size_t> dis(0, directions.size() - 1);
	out.set_value(directions[dis(gen)]);
	return out;
}

std::map<mud::direction, mud::tile> around_tiles(
	const mud::tile& current_tile,
	const std::map<std::int64_t, mud::tile>& id_tiles)
{
	std::map<mud::direction, mud::tile> neighbour;
	for (const auto& location : current_tile.neighbours())
	{
		mud::tile tile = id_tiles.find(location.id())->second;
		neighbour.insert({ location.direction(), tile });
	}
	return neighbour;
}

bool is_tile_empty(const mud::tile& tile)
{
	return
		tile.type() == mud::tile::EMPTY &&
		tile.occupant_type() == mud::tile::NOBODY &&
		tile.occupant_id() == 0;
}

bool is_tile_empty_or_character(const mud::tile& tile)
{
	return
		tile.type() == mud::tile::EMPTY &&
		((tile.occupant_type() == mud::tile::NOBODY &&
			tile.occupant_id() == 0) ||
			(tile.occupant_type() == mud::tile::CHARACTER &&
				tile.occupant_id() != 0));
}

bool is_tile_empty_or_enemy(const mud::tile& tile)
{
	return
		tile.type() == mud::tile::EMPTY &&
		((tile.occupant_type() == mud::tile::NOBODY &&
			tile.occupant_id() == 0) ||
			(tile.occupant_type() == mud::tile::ENEMY &&
				tile.occupant_id() != 0));
}

static bool check_in_out(const std::vector<mud::direction>& v)
{
	{
		auto it1 = std::find_if(
			v.begin(), 
			v.end(), 
			[](const mud::direction& d) { return d == direction::north; });
		auto it2 = std::find_if(
			v.begin(), 
			v.end(), 
			[](const mud::direction& d) { return d == direction::south; });
		if (it1 != v.end() &&	it2 != v.end()) return true;
	}
	{
		auto it1 = std::find_if(
			v.begin(), 
			v.end(), 
			[](const mud::direction& d) { return d == direction::west; });
		auto it2 = std::find_if(
			v.begin(), 
			v.end(), 
			[](const mud::direction& d) { return d == direction::east; });
		if (it1 != v.end() &&	it2 != v.end()) return true;
	}
	return false;
}

static bool check_stack(const std::vector<mud::direction>& stack)
{
	std::map<mud::direction, int> direction_count_map;
	mud::direction current_direction = get_invert_direction(stack.front());
	int current_max = 0;
	int current_count = 0;
	for (const auto& element : stack)
	{
		if (current_direction.value() != element.value())
		{
			current_direction.set_value(element.value());
			current_count = 0;
		}
		if (direction_count_map.find(element) == direction_count_map.end())
		{
			direction_count_map.insert({ element, 1 });
			current_count = 1;
		}
		else
		{
			direction_count_map[element]++;
			current_count++;
		}
		if (current_max < current_count)
		{
			current_max = current_count;
		}
	}
	if (direction_count_map.size() == 1) return true;
	if (direction_count_map.size() != 2) return false;
	{
		std::vector<mud::direction> v{};
		for (const auto& field : direction_count_map)
		{
			v.push_back(field.first);
		}
		if (check_in_out(v)) return false;
	}
	auto result = std::minmax_element(
		direction_count_map.begin(),
		direction_count_map.end(),
		[](const auto& l, const auto& r)
	{
		return l.second < r.second;
	});
	const float value = 
		static_cast<float>(result.second->second) / 
		static_cast<float>(result.first->second + 1);
	const float max = static_cast<float>(current_max);
	if ((value <= max) && (value + 1.f >= max))
	{
		return true;
	}
	return false;
}

static void see_around_tile_recurse(
	const mud::tile& current_tile,
	const std::map<std::int64_t, mud::tile>& id_tiles,
	const int range,
	std::vector<std::pair<mud::direction, mud::tile>>& initial_set,
	const std::vector<mud::direction>& direction_stack)
{
	if (range == 0) return;
	for (const auto& field : around_tiles(current_tile, id_tiles))
	{
		if (std::find_if(
			initial_set.begin(),
			initial_set.end(),
			[&field](const auto& element) 
		{
			return 
				(field.first == element.first) && 
				(field.second.id() == element.second.id());
		}) != initial_set.end())
		{
			continue;
		}
		std::vector<mud::direction> stack = direction_stack;
		stack.push_back(field.first);
		if (is_tile_empty_or_character(field.second) && check_stack(stack))
		{
			initial_set.push_back({ stack.front(), field.second });
			see_around_tile_recurse(
				field.second, 
				id_tiles, 
				range - 1, 
				initial_set, 
				stack);
		}
	}
}

std::vector<std::pair<mud::direction, mud::tile>> see_around_tiles(
	const mud::tile& current_tile,
	const std::map<std::int64_t, mud::tile>& id_tiles,
	const int range)
{
	std::vector<std::pair<mud::direction, mud::tile>> see_tile_set = {};
	std::vector<mud::direction> direction_stack;
	see_around_tile_recurse(
		current_tile, 
		id_tiles, 
		range, 
		see_tile_set, 
		direction_stack);
	return see_tile_set;
}
