#pragma once
#include "metainfo.h"
#include <openssl/sha.h>

class Torrent {
public:
  MetaInfo metaInfo;

  Torrent(MetaInfo info) : metaInfo(info) {};

  void announce() {};
  void seed() {}
  void download() {}
};
