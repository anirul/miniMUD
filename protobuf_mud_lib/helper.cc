#include "helper.h"

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
	std::for_each(
		character.attributes().begin(),
		character.attributes().end(),
		[&os](const mud::attribute& attribute)
	{
		os << "\tname      : " << attribute.name() << std::endl;
		os << "\tvalue     : " << attribute.value() << std::endl;
		os << "\tregen     : " << attribute.regen() << std::endl;
	});
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
	for (const auto& field : tile.neighbours()) {
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

std::ostream& operator<< (std::ostream& os, const input& key)
{
	switch (key)
	{
	case input::ATTACK:
		os << "ATTACK";
		break;
	case input::BACKWARD:
		os << "BACKWARD";
		break;
	case input::FORWARD:
		os << "FORWARD";
		break;
	case input::INFO:
		os << "INFO";
		break;
	case input::LEFT:
		os << "LEFT";
		break;
	case input::NONE:
		os << "NONE";
		break;
	case input::PRINT:
		os << "PRINT";
		break;
	case input::QUIT:
		os << "QUIT";
		break;
	case input::RIGHT:
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