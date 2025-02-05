#pragma once

#include <iostream>
#include <istream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// BEncoding is the format used to encode information in the .torrent files
// see https://wiki.theory.org/BitTorrentSpecification#Bencoding

struct BObj;

struct BDict {
  // we store keys in order
  // ordering of keys is important for the protocol
  std::vector<std::string> keys;
  std::unordered_map<std::string, BObj> map;
};

struct BObj {
  std::variant<int64_t, std::string, std::vector<BObj>, BDict> inner;
  // start position of object in the string stream and its length
  // here we need to be able to recover the string for the info dictionary
  int pos;
  int length;

  static BObj fromInt(int64_t a);
  static BObj fromString(std::string a);
  static BObj fromDict(BDict a);
  static BObj fromVec(std::vector<BObj> a);
};

BObj decodeObject(std::istream &stream);

int64_t decodeInt(std::istream &stream, char end);

std::string decodeString(std::istream &stream);

std::vector<BObj> decodeList(std::istream &stream);

BDict decodeDict(std::istream &stream);

class BEncodingException : public std::runtime_error {
  std::string msg;

public:
  const char *what() const noexcept override { return msg.c_str(); }
};

std::string encodeObject(BObj &ojb);
