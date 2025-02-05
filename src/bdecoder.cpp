#include "bdecoder.h"
#include <cctype>
#include <cstdlib>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

// BEncoding is the format used to encode information in the .torrent files
// Text from https://wiki.theory.org/BitTorrentSpecification#Bencoding

BObj BObj::fromInt(int64_t a) {
  return BObj{
      .inner = std::variant<int64_t, std::string, std::vector<BObj>, BDict>(a)};
}

BObj BObj::fromString(std::string a) {
  return BObj{
      .inner = std::variant<int64_t, std::string, std::vector<BObj>, BDict>(a)};
}

BObj BObj::fromDict(BDict a) {
  return BObj{
      .inner = std::variant<int64_t, std::string, std::vector<BObj>, BDict>(a)};
}

BObj BObj::fromVec(std::vector<BObj> a) {
  return BObj{
      .inner = std::variant<int64_t, std::string, std::vector<BObj>, BDict>(a)};
}

BObj decodeObject(std::istream &stream) {
  char inp = stream.peek();
  int start = stream.tellg();

  BObj obj;

  if (inp == 'i') {
    stream.get();
    obj = BObj::fromInt(decodeInt(stream, 'e'));
  } else if (inp == 'd') {
    stream.get();
    obj = BObj::fromDict(decodeDict(stream));
  } else if (inp == 'l') {
    stream.get();
    obj = BObj::fromVec(decodeList(stream));
  } else if (std::isalnum(inp)) {
    obj = BObj::fromString(decodeString(stream));
  } else {
    // we weren't able to identify a bencoded object
    throw std::runtime_error(std::format(
        "Error parsing BEncoding object at position {} char {}", start, inp));
  }

  int end = stream.tellg();
  obj.pos = start;
  obj.length = end - start;

  return obj;
}

BDict decodeDict(std::istream &stream) {
  std::unordered_map<std::string, BObj> map;
  std::vector<std::string> keys;

  while (stream.peek() != 'e') {
    auto key = decodeString(stream);
    auto value = decodeObject(stream);
    keys.push_back(key);
    map[key] = value;
  }

  stream.get(); // clear 'e' from stream

  return BDict{.keys = keys, .map = map};
}

std::vector<BObj> decodeList(std::istream &stream) {
  std::vector<BObj> list;

  while (stream.peek() != 'e') {
    list.push_back(decodeObject(stream));
  }

  stream.get(); // clear 'e' from stream
  return list;
}

int64_t decodeInt(std::istream &stream, char end) {
  char buf[26] = {'\0'};
  char *endptr = &buf[0];
  stream.getline(&buf[0], 25, end);

  int64_t i = strtoll(&buf[0], &endptr, 10);

  if (*endptr != '\0') { // we weren't able to read the whole string
    int pos = stream.tellg() - stream.gcount();
    throw std::runtime_error(std::format(
        "Error parsing BEncoding integer literal {} at position {}", buf, pos));
  }

  return i;
}

std::string decodeString(std::istream &stream) {
  int length = decodeInt(stream, ':');
  std::string str(length, '\0');
  stream.read(&str[0], length);
  return str;
}
