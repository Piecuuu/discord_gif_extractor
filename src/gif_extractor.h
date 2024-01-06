#include <vector>
#include <argparse/argparse.hpp>
#include <string>

namespace GifExtractor {
std::string remove_prefix_and_suffix(std::string& string, std::string prefix, std::string suffix);
std::string get_data_by_token(const std::string& token);
std::string protobuf_to_json(const std::string& protobuf_data);
std::vector<std::string> parse_json(const std::string& json_str);
void write_links_to_stdout(std::vector<std::string> links);
void write_links_to_file(std::vector<std::string> links, std::string linksfile_path);
void write_links_from_data(std::string& data, argparse::ArgumentParser& parser, std::string& outputfile_path, bool write_to_stdout);

}
