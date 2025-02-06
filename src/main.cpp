#include "metainfo.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <iomanip>

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
    auto metainfo = MetaInfo::FromStream(stream);

    auto &fileInfo = metainfo.files[0];
    std::cout << "name         " << metainfo.name << std::endl;
    std::cout << "announce     " << metainfo.announce << std::endl;
    std::cout << "pieceLength  " << metainfo.pieceLength << std::endl;
    std::cout << "comment      " << metainfo.comment << std::endl;
    std::cout << "createdBy    " << metainfo.createdBy << std::endl;
    std::cout << "creationDate " << metainfo.creationDate << std::endl;

    for (auto &file : metainfo.files) {
      std::cout << "FILE" << std::endl;
      std::cout << "path         " << file.path << std::endl;
      std::cout << "length       " << file.length << std::endl;
    }

    // for (auto &hash : metainfo.pieces) {
    //   std::cout << "PIECE" << std::endl;
    //   for (auto byte : hash) {
    //     std::cout
    //         << std::hex << std::setw(2) << std::setfill('0') << (int)byte
    //         << " ";
    //   }
    //   std::cout << std::endl;
    // }

    std::cout << "INFOHASH" << std::endl;

    for (auto byte : metainfo.infoHash) {
      std::cout
          << std::hex << std::setw(2) << std::setfill('0') << (int)byte
          << " ";
    }
    std::cout << std::endl;

    std::cout << "ANNOUNCE" << std::endl;
    std::cout << announce(metainfo);

  } catch (std::runtime_error msg) {
    std::cerr << msg.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  return 0;
}
