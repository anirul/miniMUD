#pragma once
#include <string>
#include <fstream>
#include <functional>
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable: 4005)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996)
#endif
#include "mud_lib.pb.h"
#include <google/protobuf/util/json_util.h>
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif

enum class input {
	NONE = 0,
	FORWARD = 1,
	BACKWARD = 2,
	LEFT = 3,
	RIGHT = 4,
	ATTACK = 5,
	QUIT = 6,
	INFO = 7,
	PRINT = 8,
};

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

std::ostream& operator<< (std::ostream& os, const mud::player& player);
std::ostream& operator<< (std::ostream& os, const mud::character& character);
std::ostream& operator<< (std::ostream& os, const mud::tile& tile);

std::ostream& operator<< (std::ostream& os, const mud::direction& direction);
std::ostream& operator<< (std::ostream& os, const mud::character_state& state);
std::ostream& operator<< (std::ostream& os, const mud::resident_type& resident);
std::ostream& operator<< (std::ostream& os, const mud::tile_type& tile);
std::ostream& operator<< (std::ostream& os, const mud::attribute_name& name);

std::ostream& operator<< (std::ostream& os, const input& key);

mud::direction get_invert_direction(const mud::direction& dir);
mud::direction get_left_direction(const mud::direction& dir);
mud::direction get_right_direction(const mud::direction& dir);

std::map<mud::direction, mud::tile> around_tiles(
	const mud::tile& current_tile,
	const std::map<std::int64_t, mud::tile>& id_tiles);

