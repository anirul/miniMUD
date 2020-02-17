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
	// size to           :
	os << "name          : " << player.name() << std::endl;
	os << "password      : " << player.password_hash() << std::endl;
	os << "id            : " << player.id() << std::endl;
	os << "characters(" << player.id_characters().size() << ")" << std::endl;
	bool first = true;
	for (const auto& id : player.id_characters())
	{
		if (first)
		{
			first = false;
			os << "\t";
		}
		else
		{
			os << ", ";
		}
		os << id;
	}
	os << std::endl;
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::character& character)
{
	// size to           :
	os << "name          : " << character.name() << std::endl;
	os << "id            : " << character.id() << std::endl;
	os << "tile id       : " << character.tile_id() << std::endl;
	os << "facing        : " << character.facing() << std::endl;
	os << "state         : " << character.state() << std::endl;
	os << "attributes(" << character.attributes().size() << ")" << std::endl;
	for (const auto& attr : character.attributes())
	{
		os << attr << std::endl;
	}
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::tile& tile)
{
	// size to           :
	os << "mood          : " << tile.mood() << std::endl;
	os << "type          : " << tile.type() << std::endl;
	os << "id            : " << tile.id() << std::endl;
	os << "occupant type : " << tile.occupant_type() << std::endl;
	os << "occupant id   : " << tile.occupant_id() << std::endl;
	os << "neighbours(" << tile.neighbours().size() << ")" << std::endl;
	for (const auto& field : tile.neighbours()) 
	{
		os << field << std::endl;
	}
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::enemy& enemy)
{
	// size to           :
	os << "name          : " << enemy.name() << std::endl;
	os << "description   : " << enemy.description() << std::endl;
	os << "id            : " << enemy.id() << std::endl;
	os << "tile id       : " << enemy.tile_id() << std::endl;
	os << "facing        : " << enemy.facing() << std::endl;
	os << "attributes(" << enemy.attributes().size() << ")" << std::endl;
	for (const auto& attribute : enemy.attributes())
	{
		os << attribute << std::endl;
	}
	os 
		<< "droppable_item_ids(" << enemy.attributes().size() << ")" 
		<< std::endl;
	bool first = true;
	for (const auto& id_item : enemy.droppable_item_ids())
	{
		if (first)
		{
			first = false;
			os << "\t";
		}
		else
		{
			os << ", ";
		}
		os << id_item;
	}
	os << std::endl;
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::item& item)
{
	// size to           :
	os << "name          : " << item.name() << std::endl;
	os << "description   : " << item.description() << std::endl;
	os << "id            : " << item.id() << std::endl;
	os << "attributes(" << item.attributes().size() << ")" << std::endl;
	for (const auto& attribute : item.attributes())
	{
		os << attribute;
	}
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::attribute& attribute)
{
	// size to \t               :
	os << "\tname               : " << attribute.name() << std::endl;
	os << "\tscore              : " << attribute.score()
		<< " [" << attribute.score_max() << "]" << std::endl;
	os << "\tregen              : " << attribute.regen() << std::endl;
	return os;
}

std::ostream& operator<< (std::ostream& os, const mud::location& location)
{
	// size to \t               :
	os << "\tlocation direction : " << location.direction() << std::endl;
	os << "\tlocation id        : " << location.id() << std::endl;
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
	default:
		os << "unknown";
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
	default:
		os << "unknown";
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
	default:
		os << "unknown";
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
	default:
		os << "unknown";
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
	case mud::attribute::MELEE_POWER:
		os << "MELEE_POWER";
		break;
	case mud::attribute::RANGE_POWER:
		os << "RANGE_POWER";
		break;
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
	default:
		os << "unknown";
		break;
	}
	return os;
}

std::ostream& operator<<(
	std::ostream& os, 
	const mud::play_in::command_enum& command)
{
	switch (command)
	{
	case mud::play_in::NO_COMMAND:
		os << "NO_COMMAND";
		break;
	case mud::play_in::TURN_LEFT:
		os << "TURN_LEFT";
		break;
	case mud::play_in::TURN_RIGHT:
		os << "TURN_RIGHT";
		break;
	case mud::play_in::FORWARD:
		os << "FORWARD";
		break;
	case mud::play_in::BACKWARD:
		os << "BACKWARD";
		break;
	case mud::play_in::ATTACK_MELEE:
		os << "ATTACK_MELEE";
		break;
	case mud::play_in::ATTACK_RANGE:
		os << "ATTACK_RANGE";
		break;
	case mud::play_in::DEFEND:
		os << "DEFEND";
		break;
	case mud::play_in::HEAL:
		os << "HEAL";
		break;
	case mud::play_in::INFO:
		os << "INFO";
		break;
	case mud::play_in::QUIT:
		os << "QUIT";
		break;
	default:
		os << "unknown";
		break;
	}
	return os;
}

std::ostream& operator<<(
	std::ostream& os, 
	const mud::play_out::status_enum& status)
{
	switch (status)
	{
	case mud::play_out::FAILURE:
		os << "FAILURE";
		break;
	case mud::play_out::SUCCESS:
		os << "SUCCESS";
		break;
	case mud::play_out::QUIT:
		os << "QUIT";
		break;
	case mud::play_out::DEAD:
		os << "DEAD";
		break;
	default:
		os << "unknown";
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

std::map<std::int64_t, mud::direction> around_tiles(
	std::int64_t id_tile_current,
	const std::unordered_map<std::int64_t, mud::tile>& id_tiles)
{
	std::map<std::int64_t, mud::direction> neighbour;
	auto it = id_tiles.find(id_tile_current);
	if (it != id_tiles.end())
	{
		for (const auto& location : it->second.neighbours())
		{
			neighbour.insert({ location.id(), location.direction() });
		}
	}
	return neighbour;
}

std::map<std::int64_t, mud::direction> see_around_tiles(
	std::int64_t id_tile_current,
	const std::unordered_map<std::int64_t, mud::tile>& id_tiles,
	const int range)
{
	if (range <= 0) return {};
	std::map<std::int64_t, mud::direction> out{};
	for (const auto& field : around_tiles(id_tile_current, id_tiles))
	{
		if (field.first == 0) continue;
		if (id_tiles.find(field.first) == id_tiles.end()) continue;
		out.insert({ field.first, field.second });
		auto new_map = see_around_tiles(field.first, id_tiles, range - 1);
		for (const auto& in_field : new_map)
		{
			if (in_field.first == id_tile_current) continue;
			out.insert({ in_field.first, field.second });
		}
	}
	return out;
}
