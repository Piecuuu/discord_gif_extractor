#include <iostream>
#include <vector>
#include <FrecencyUserSettings.pb.h>
#include <base64pp.h>
#include <google/protobuf/util/json_util.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <argparse/argparse.hpp>

std::string protobuf_to_json(const std::string& protobuf_data) {
  auto const decoded_data = base64pp::decode(protobuf_data);
  std::string string_data(decoded_data.value().begin(), decoded_data.value().end());

  discord_protos::discord_users::v1::FrecencyUserSettings user_settings;
  user_settings.ParseFromString(string_data);

  std::string json_data;
  google::protobuf::util::JsonPrintOptions options;
  options.add_whitespace = false;
  options.always_print_primitive_fields = true;
  google::protobuf::util::MessageToJsonString(user_settings, &json_data, options);

  return json_data;
}

std::vector<std::string> parse_json(const std::string& json_str) {
  std::vector<std::string> links;
  nlohmann::json json = nlohmann::json::parse(json_str);
  nlohmann::json gifs = json["favoriteGifs"]["gifs"];
  for(auto it = gifs.begin(); it != gifs.end(); it++) {
    links.push_back(it.key());
  }
  return links;
}

void write_links_to_stdout(std::vector<std::string> links) {
  for(auto& link : links) {
    std::cout << link << "\n";
  }
}

void write_links_to_file(std::vector<std::string> links, std::string linksfile_path) {
  std::ofstream linksfile(linksfile_path);
  for(auto& link : links) {
    linksfile << link << "\n";
  }
  linksfile.close();
}

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("discord_gif_extractor");
  program.add_argument("-o", "--output")
    .help("File to output to")
    .default_value(std::string("links.txt"));
  program.add_argument("-i", "--input")
    .help("File to get data from")
    .default_value(std::string("data.txt"));
  program.add_argument("-d", "--decode-only")
    .help("Only decode protobuf and output JSON data")
    .flag();

  try {
    program.parse_args(argc, argv);    // Example: ./main --color red --color green --color blue
  }
  catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  std::string outputfile_path = program.get<std::string>("--output");
  std::string datafile_path = program.get<std::string>("--input");
  bool write_to_stdout = outputfile_path == "-";

  std::string data;
  //std::cin >> data;
  std::ifstream datafile(datafile_path);
  std::getline(datafile, data);

  std::string json_data = protobuf_to_json(data);
  datafile.close();

  if(program["--decode-only"] == true) {
    if(write_to_stdout) {
      std::cout << json_data << "\n";
      return 0;
    }
    std::ofstream outputfile(outputfile_path);
    outputfile << json_data << "\n";
    outputfile.close();
    return 0;
  }

  std::vector<std::string> links = parse_json(json_data);
  write_to_stdout ? write_links_to_stdout(links) : write_links_to_file(links, outputfile_path);

  //std::cout << json_data << "\n";
  return 0;
}
