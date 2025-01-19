#include "bdecoder.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Missing .torrent file and output folder" << std::endl;
    exit(1);
  }

  std::cout << ".torrent input file is " << argv[1] << std::endl;
  std::cout << "Output folder is " << argv[2] << std::endl;

  // testing BEnconding
  int64_t other;

  try {
    std::istringstream stream1("i1203e");
    other = std::get<int64_t>(decodeObject(stream1).inner);
    std::cout << other << std::endl;

    std::istringstream stream("i0monkeys");
    other = std::get<int64_t>(decodeObject(stream).inner);
  } catch (std::runtime_error msg) {
    std::cout << msg.what() << std::endl;
    exit(1);
  }

  /*for (auto obj : other) {*/
  /*  std::cout << obj.first << std::get<std::string>(obj.second.inner) << '
   * ';*/
  /*}*/

  std::cout << std::endl;

  return 0;
}
