#include "helper.h"
#include <stack>
#include <array>
#include <algorithm>
#include <random>

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
		os << "\tvalue     : " << attr.value() << std::endl;
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
	switch (direction) 
	{
	case mud::NORTH:
		os << "NORTH";
		break;
	case mud::SOUTH:
		os << "SOUTH";
		break;
	case mud::WEST:
		os << "WEST";
		break;
	case mud::EAST:
		os << "EAST";
		break;
	}
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::character_state& state)
{
	switch (state) 
	{
	case mud::NONE:
		os << "NONE";
		break;
	case mud::WALKING:
		os << "WALKING";
		break;
	case mud::COMBAT:
		os << "COMBAT";
		break;
	}
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::resident_type& resident)
{
	switch (resident)
	{
	case mud::NOBODY:
		os << "NOBODY";
		break;
	case mud::CHARACTER:
		os << "CHARACTER";
		break;
	case mud::ENEMY:
		os << "ENEMY";
		break;
	}
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::tile_type& tile)
{
	switch (tile)
	{
	case mud::EMPTY:
		os << "EMPTY";
		break;
	case mud::WALL:
		os << "WALL";
		break;
	case mud::TREE:
		os << "TREE";
		break;
	case mud::PORTAL:
		os << "PORTAL";
		break;
	}
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::attribute_name& name)
{
	switch (name)
	{
	case mud::LIFE:
		os << "LIFE";
		break;
	case mud::STRENGTH:
		os << "STRENGTH";
		break;
	case mud::AGILITY:
		os << "AGILITY";
		break;
	case mud::INTELLIGENCE:
		os << "INTELLIGENCE";
		break;
	}
	return os;
}

std::ostream& operator<< (std::ostream& os, const input_t& key)
{
	switch (key)
	{
	case input_t::ATTACK:
		os << "ATTACK";
		break;
	case input_t::BACKWARD:
		os << "BACKWARD";
		break;
	case input_t::FORWARD:
		os << "FORWARD";
		break;
	case input_t::INFO:
		os << "INFO";
		break;
	case input_t::LEFT:
		os << "LEFT";
		break;
	case input_t::NONE:
		os << "NONE";
		break;
	case input_t::PRINT:
		os << "PRINT";
		break;
	case input_t::QUIT:
		os << "QUIT";
		break;
	case input_t::RIGHT:
		os << "RIGHT";
		break;
	}
	return os;
}

mud::direction get_invert_direction(const mud::direction& dir)
{
	switch (dir)
	{
	case mud::NORTH:
		return mud::WEST;
	case mud::SOUTH:
		return mud::EAST;
	case mud::EAST:
		return mud::NORTH;
	case mud::WEST:
	default:
		return mud::SOUTH;
	}
}

mud::direction get_left_direction(const mud::direction& dir)
{
	switch (dir)
	{
	case mud::NORTH:
		return mud::WEST;
	case mud::SOUTH:
		return mud::EAST;
	case mud::EAST:
		return mud::NORTH;
	case mud::WEST:
	default:
		return mud::SOUTH;
	}
}

mud::direction get_right_direction(const mud::direction& dir)
{
	switch (dir)
	{
	case mud::NORTH:
		return mud::EAST;
	case mud::SOUTH:
		return mud::WEST;
	case mud::EAST:
		return mud::SOUTH;
	case mud::WEST:
	default:
		return mud::NORTH;
	}
}

mud::direction get_random_direction() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::vector<mud::direction> directions = 
		{ mud::NORTH, mud::SOUTH, mud::WEST, mud::EAST };
	std::uniform_int_distribution<size_t> dis(0, directions.size() - 1);
	return directions[dis(gen)];
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

static bool check_stack(const std::vector<mud::direction>& stack)
{
	std::map<mud::direction, int> direction_count_map;
	mud::direction current_direction = get_invert_direction(stack.front());
	int current_max = 0;
	int current_count = 0;
	for (const auto& element : stack)
	{
		if (current_direction != element)
		{
			current_direction = element;
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
	if (direction_count_map.size() > 2) return false;
	if (direction_count_map.size() == 1) return true;
	auto result = std::minmax(
		direction_count_map.begin(), 
		direction_count_map.end(), 
		[](const auto& l, const auto& r)
	{
		return l->second < r->second;
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
		}) == initial_set.end())
		{
			continue;
		}
		std::vector<mud::direction> stack = direction_stack;
		stack.push_back(field.first);
		if (field.second.type() == mud::EMPTY && check_stack(stack))
		{
			initial_set.push_back(field);
			see_around_tile_recurse(
				field.second, 
				id_tiles, 
				range - 1, 
				initial_set, 
				direction_stack);
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
		current_tile, id_tiles, 
		range, 
		see_tile_set, 
		direction_stack);
	return see_tile_set;
}
