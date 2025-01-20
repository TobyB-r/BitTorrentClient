#pragma once

#include <iostream>
#include <istream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
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
struct BObj;

struct BDict {
  // we store keys in order
  // ordering of keys is important for the protocol
  std::vector<std::string> keys;
  std::unordered_map<std::string, BObj> map;
};

struct BObj {
  std::variant<int64_t, std::string, std::vector<BObj>, BDict> inner;

  static BObj fromInt(int64_t a);
  static BObj fromString(std::string a);
  static BObj fromDict(BDict a);
  static BObj fromVec(std::vector<BObj> a);
};

BObj decodeObject(std::basic_istream<char> &stream);

int64_t decodeInt(std::basic_istream<char> &stream, char end);

std::string decodeString(std::basic_istream<char> &stream);

std::vector<BObj> decodeList(std::basic_istream<char> &stream);

BDict decodeDict(std::basic_istream<char> &stream);

class BEncodingException : public std::runtime_error {
  std::string msg;

public:
  const char *what() const noexcept override { return msg.c_str(); }
};
