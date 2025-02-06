#pragma once
#include "bdecoder.h"
#include <cstdint>
#include <ctime>
#include <string>
#include <vector>
#include <array>

// the metainfo is the bencoded information in the .torrent file
// see https://wiki.theory.org/BitTorrentSpecification#Metainfo_File_Structure

typedef std::array<uint8_t, 20> sha1_hash_t;

class FileInfo {
public:
  int64_t length;
  std::string path;

  FileInfo(int64_t length, std::string path);
};

class MetaInfo {
public:
  std::string name;
  std::string announce;
  bool singleFile;

  // optional elements
  std::vector<std::string> announceList;
  time_t creationDate;
  std::string comment;
  std::string createdBy;
  std::string encoding;

  // Info dictionary elements
  int64_t pieceLength;
  std::vector<sha1_hash_t> pieces;
  bool isprivate;     // optional
  sha1_hash_t infoHash; // sha1 hash of file

  std::vector<FileInfo> files;

  static MetaInfo New(BDict &dict);

  static MetaInfo FromStream(std::istream &in);
};

// Makes http get request to tracker to announce our joining
// see
// https://wiki.theory.org/BitTorrentSpecification#Tracker_HTTP.2FHTTPS_Protocol
std::string announce(MetaInfo& info);

