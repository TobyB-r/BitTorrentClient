#include "metainfo.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Missing .torrent file and output folder" << std::endl;
    exit(1);
  }

  std::cout << ".torrent input file is " << argv[1] << std::endl;
  std::cout << "Output folder is " << argv[2] << std::endl;

  // testing MetaInfo

  try {
    auto stream = std::ifstream(argv[1], std::ios::binary);
    auto metaInfo = MetaInfo::FromStream(stream);
    auto &fileInfo = metaInfo.files[0];
    std::cout << "name         " << metaInfo.name << std::endl;
    std::cout << "announce     " << metaInfo.announce << std::endl;
    std::cout << "pieceLength  " << metaInfo.pieceLength << std::endl;
    std::cout << "comment      " << metaInfo.comment << std::endl;
    std::cout << "createdBy    " << metaInfo.createdBy << std::endl;
    std::cout << "creationDate " << metaInfo.creationDate << std::endl;

    for (auto &file : metaInfo.files) {
      std::cout << "FILE" << std::endl;
      std::cout << "path         " << file.path << std::endl;
      std::cout << "length       " << file.length << std::endl;
    }

    std::cout << "INFO" << std::endl;
    std::cout << metaInfo.infoString;

  } catch (std::runtime_error msg) {
    std::cerr << msg.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  return 0;
}
