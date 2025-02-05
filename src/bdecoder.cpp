#include "bdecoder.h"
#include <cctype>
#include <cstdlib>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

/* BEncoding is the format used to encode information in the .torrent files
 * Text from https://wiki.theory.org/BitTorrentSpecification#Bencoding

 Byte Strings

 Byte strings are encoded as follows: <string length encoded in base ten
 ASCII>:<string data> Note that there is no constant beginning delimiter, and no
 ending delimiter.

     Example: 4: spam represents the string "spam"
     Example: 0: represents the empty string ""

 Integers

 Integers are encoded as follows: i<integer encoded in base ten ASCII>e
 The initial i and trailing e are beginning and ending delimiters.

     Example: i3e represents the integer "3"
     Example: i-3e represents the integer "-3"

 i-0e is invalid. All encodings with a leading zero, such as i03e, are invalid,
 other than i0e, which of course corresponds to the integer "0".

     NOTE: The maximum number of bit of this integer is unspecified, but to
 handle it as a signed 64bit integer is mandatory to handle "large files" aka
 .torrent for more that 4Gbyte.

 Lists

 Lists are encoded as follows: l<bencoded values>e
 The initial l and trailing e are beginning and ending delimiters. Lists may
 contain any bencoded type, including integers, strings, dictionaries, and even
 lists within other lists.

     Example: l4:spam4:eggse represents the list of two strings: [ "spam",
 "eggs" ] Example: le represents an empty list: []

 Dictionaries

 Dictionaries are encoded as follows: d<bencoded string><bencoded element>e
 The initial d and trailing e are the beginning and ending delimiters. Note that
 the keys must be bencoded strings. The values may be any bencoded type,
 including integers, strings, lists, and other dictionaries. Keys must be
 strings and appear in sorted order (sorted as raw strings, not alphanumerics).
 The strings should be compared using a binary comparison, not a
 culture-specific "natural" comparison.

     Example: d3:cow3:moo4:spam4:eggse represents the dictionary { "cow" =>
 "moo", "spam" => "eggs" }

     Example: d4:spaml1:a1:bee represents the dictionary {
 "spam" => [ "a", "b" ] }

     Example: de represents an empty dictionary {}
 */

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
