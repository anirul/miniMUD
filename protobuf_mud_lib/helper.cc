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