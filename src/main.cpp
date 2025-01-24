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
  int64_t other;

  try {
    auto stream = std::ifstream(argv[1], std::ios::binary);
    auto metaInfo = MetaInfo::FromStream(stream);
    auto fileInfo = metaInfo.files[0];

    std::cout << "name         " << metaInfo.name << std::endl;
    std::cout << "announce     " << metaInfo.announce << std::endl;
    std::cout << "pieceLength  " << metaInfo.pieceLength << std::endl;
    std::cout << "comment      " << metaInfo.comment << std::endl;
    std::cout << "createdBy    " << metaInfo.createdBy << std::endl;
    std::cout << "creationDate " << metaInfo.creationDate << std::endl;
    std::cout << "fileLength   " << fileInfo.length << std::endl;

  } catch (std::runtime_error msg) {
    std::cout << msg.what() << std::endl;
    exit(1);
  }

  return 0;
}
