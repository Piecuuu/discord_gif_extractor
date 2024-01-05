#include <iostream>
#include <vector>
#include <FrecencyUserSettings.pb.h>
#include <base64pp.h>
#include <google/protobuf/util/json_util.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <argparse/argparse.hpp>
#include <curl/curl.h>

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

std::string remove_prefix_and_suffix(std::string& string, std::string prefix, std::string suffix) {
  size_t start_pos = string.find(prefix);
  size_t end_pos = string.rfind(suffix);
  std::string result = string.substr(start_pos + prefix.length(), end_pos - start_pos - prefix.length());
  return result;
}

std::string get_data_by_token(const std::string& token) {
  CURL *curl;
  CURLcode res;
  std::string data;

  struct curl_slist *list = NULL;
  curl = curl_easy_init();
  if(curl) {
    list = curl_slist_append(list, std::format("authorization: {}", token).c_str());
    list = curl_slist_append(list, "Accept: */*");

    curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/v9/users/@me/settings-proto/2");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/121.0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
    res = curl_easy_perform(curl);
    if(res == CURLM_OK) {
      return remove_prefix_and_suffix(data, "{\"settings\":\"", "\"}");
    } else {
      throw std::runtime_error("CURL has returned a non-ok status");
    }
    curl_easy_cleanup(curl);
    curl_slist_free_all(list);
  } else {
    throw std::runtime_error("Couldn't initiate CURL");
  }
  return "";
}

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

void write_links_from_data(std::string& data, argparse::ArgumentParser& parser, std::string& outputfile_path, bool write_to_stdout) {
  std::string json_data = protobuf_to_json(data);

  if(parser["--decode-only"] == true) {
    if(write_to_stdout) {
      std::cout << json_data << "\n";
      return;
    }
    std::ofstream outputfile(outputfile_path);
    outputfile << json_data << "\n";
    outputfile.close();
    return;
  }

  std::vector<std::string> links = parse_json(json_data);
  write_to_stdout ? write_links_to_stdout(links) : write_links_to_file(links, outputfile_path);
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
  program.add_argument("-t", "--token")
    .help("If set, the program will automatically fetch the data");

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
  bool get_from_stdin = datafile_path == "-";

  if(program.present("--token")) {
    std::string data = get_data_by_token(program.get<std::string>("--token"));
    write_links_from_data(data, program, outputfile_path, write_to_stdout);
    return 0;
  }

  std::string data;
  if(get_from_stdin) {
    std::cin >> data;
  } else {
    std::ifstream datafile(datafile_path);
    std::getline(datafile, data);
    datafile.close();
  }

  write_links_from_data(data, program, outputfile_path, write_to_stdout);
  return 0;
}
