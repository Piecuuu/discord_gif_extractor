#include <iostream>
#include <vector>
#include <FrecencyUserSettings.pb.h>
#include <base64pp.h>
#include <google/protobuf/util/json_util.h>
#include <nlohmann/json.hpp>
#include <fstream>

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

int main() {
  std::string data;
  //std::cin >> data;
  std::ifstream datafile("data.txt");
  std::getline(datafile, data);

  std::string json_data = protobuf_to_json(data);
  datafile.close();

  std::vector<std::string> links = parse_json(json_data);
  std::ofstream linksfile("links.txt");
  for(auto& link : links) {
    linksfile << link << "\n";
  }
  linksfile.close();

  //std::cout << json_data << "\n";
  return 0;
}
