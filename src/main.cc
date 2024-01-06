#include <iostream>
#include "gif_extractor.h"
#include <fstream>

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
    std::string data = GifExtractor::get_data_by_token(program.get<std::string>("--token"));
    GifExtractor::write_links_from_data(data, program, outputfile_path, write_to_stdout);
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

  GifExtractor::write_links_from_data(data, program, outputfile_path, write_to_stdout);
  return 0;
}
