#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <functional>
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#endif
#include "../protobuf_mud_lib/mud_lib.pb.h"
#include <google/protobuf/util/json_util.h>
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif

template <typename Book>
Book read_file(const std::string& filename)
{
	Book book;
	std::ifstream ifs(filename, std::ios::in | std::ios::binary);
	if (!ifs)
	{
		std::cerr
			<< filename
			<< " : file cannot be opened?"
			<< std::endl;
	}
	else
	{
		book.ParseFromIstream(&ifs);
	}
	return book;
}

template <typename Book>
Book read_json(const std::string& filename)
{
	Book book;
	std::ifstream ifs(filename, std::ios::in);
	if (!ifs)
	{
		std::cerr
			<< filename
			<< " : file cannot be converted to json."
			<< std::endl;
	}
	else
	{
		std::string contents(std::istreambuf_iterator<char>(ifs), {});
		google::protobuf::util::JsonParseOptions options{};
		options.ignore_unknown_fields = true;
		auto status = 
			google::protobuf::util::JsonStringToMessage(
				contents, 
				&book, 
				options);
		if (!status.ok()) 
		{
			std::cout << status << std::endl;
		}
	}
	return book;
}

template <typename Book>
void write_file(const std::string& filename, const Book& book)
{
	std::ofstream ofs(filename, std::ios::out | std::ios::binary);
	if (!book.SerializeToOstream(&ofs))
	{
		std::cerr << "error outputing to file: " << filename << std::endl;
	}
}

template <typename Book>
void write_json(const std::string& filename, const Book& book)
{
	std::ofstream ofs(filename, std::ios::out);
	std::string out;
	google::protobuf::util::JsonOptions options{};
	options.add_whitespace = true;
	options.always_print_primitive_fields = true;
	options.always_print_enums_as_ints = false;
	options.preserve_proto_field_names = true;
	auto status = 
		google::protobuf::util::MessageToJsonString(book, &out, options);
	if (!status.ok()) 
	{
		std::cout << status << std::endl;
	}
	ofs << out;
	ofs.flush();
}

// For std::map<mud::direction, ...>.
template<>
struct std::less<mud::direction> {
	bool operator()(
		const mud::direction& l, 
		const mud::direction& r) const
	{
		return l.value() < r.value();
	}
};

// Some direction constants.
namespace direction {

	const mud::direction north = []
	{
		mud::direction d{};
		d.set_value(mud::direction::NORTH);
		return d;
	}();

	const mud::direction south = []
	{
		mud::direction d{};
		d.set_value(mud::direction::SOUTH);
		return d;
	}();

	const mud::direction west = []
	{
		mud::direction d{};
		d.set_value(mud::direction::WEST);
		return d;
	}();

	const mud::direction east = []
	{
		mud::direction d{};
		d.set_value(mud::direction::EAST);
		return d;
	}();

} // End namespace direction.

bool operator==(const mud::direction& l, const mud::direction& r);
bool operator!=(const mud::direction& l, const mud::direction& r);

std::ostream& operator<< (std::ostream& os, const mud::player& player);
std::ostream& operator<< (std::ostream& os, const mud::character& character);
std::ostream& operator<< (std::ostream& os, const mud::tile& tile);

std::ostream& operator<< (std::ostream& os, const mud::direction& direction);
std::ostream& operator<< (
	std::ostream& os, 
	const mud::character::character_state_enum& state);
std::ostream& operator<< (
	std::ostream& os, 
	const mud::tile::resident_type_enum& resident);
std::ostream& operator<< (
	std::ostream& os,
	const mud::tile::tile_type_enum& tile);
std::ostream& operator<< (
	std::ostream& os, 
	const mud::attribute::attribute_name_enum& name);

// Get the direction according to present one.
mud::direction get_invert_direction(const mud::direction& dir);
mud::direction get_left_direction(const mud::direction& dir);
mud::direction get_right_direction(const mud::direction& dir);
mud::direction get_random_direction();

// Check if a tile is empty (or there is someone).
bool is_tile_empty(const mud::tile& tile);
bool is_tile_empty_or_character(const mud::tile& tile);
bool is_tile_empty_or_enemy(const mud::tile& tile);

// Get a map of the surrounding tiles.
std::map<mud::direction, mud::tile> around_tiles(
	const mud::tile& current_tile,
	const std::map<std::int64_t, mud::tile>& id_tiles);

// Get the tiles you can see (hear) around you.
std::vector<std::pair<mud::direction, mud::tile>> see_around_tiles(
	const mud::tile& current_tile,
	const std::map<std::int64_t, mud::tile>& id_tiles,
	const int range = 2);
