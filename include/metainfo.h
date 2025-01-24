#pragma once
#include "bdecoder.h"
#include <cstdint>
#include <ctime>
#include <string>
#include <variant>
#include <vector>

/* the metainfo is the bencoded information in the .torrent file
 Text from
https://wiki.theory.org/BitTorrentSpecification#Metainfo_File_Structure Metainfo
File Structure

All data in a metainfo file is bencoded. The specification for bencoding is
defined above.

The content of a metainfo file (the file ending in ".torrent") is a bencoded
dictionary, containing the keys listed below. All character string values are
UTF-8 encoded.

info: a dictionary that describes the file(s) of the torrent. There are two
  possible forms: one for the case of a 'single-file' torrent with no directory
  structure, and one for the case of a 'multi-file' torrent (see below for
  details)
announce: The announce URL of the tracker (string)
announce-list: (optional) this is an extention to the official specification,
  offering backwards-compatibility. (list of lists of strings). The official
  request for a specification change is here.
creation date: (optional) the creation time of the torrent, in standard UNIX
  epoch format (integer, seconds since 1-Jan-1970 00:00:00 UTC)
comment: (optional) free-form textual comments of the author (string)
created by: (optional) name and version of the program used to create the
  .torrent (string)
encoding: (optional) the string encoding format used to
  generate the pieces part of the info dictionary in the .torrent metafile
  (string)

Info Dictionary

This section contains the field which are common to both mode, "single file" and
"multiple file".

piece length: number of bytes in each piece (integer)
pieces: string consisting of the concatenation of all 20-byte SHA1 hash
  values, one per piece (byte string, i.e. not urlencoded)
private: (optional) this field is an integer. If it is set to "1", the client
  MUST publish its presence to get other peers ONLY via the trackers explicitly
  described in the metainfo file. If this field is set to "0" or is not present,
  the client may obtain peer from other means, e.g. PEX peer exchange, dht.
  Here, "private" may be read as "no external peer source". NOTE: There is much
  debate surrounding private trackers. The official request for a specification
  change is here.

Info in Single File Mode

For the case of the single-file mode, the info dictionary contains the following
structure:

name: the filename. This is purely advisory. (string)
length: length of the file in bytes (integer)
md5sum: (optional) a 32-character hexadecimal string corresponding to the
  MD5 sum of the file. This is not used by BitTorrent at all, but it is included
  by some programs for greater compatibility.

Info in Multiple File Mode

For the case of the multi-file mode, the info dictionary contains the following
structure:

name: the name of the directory in which to store all the files. This is
  purely advisory. (string)
files: a list of dictionaries, one for each file. Each dictionary in this list
  contains the following keys:
  length: length of the file in bytes (integer)
  md5sum: (optional) a 32-character hexadecimal string corresponding to the MD5
    sum of the file. This is not used by BitTorrent at all, but it is included
    by some programs for greater compatibility.
  path: a list containing one or more
    string elements that together represent the path and filename. Each element
    in the list corresponds to either a directory name or (in the case of the
    final element) the filename. For example, a the file "dir1/dir2/file.ext"
    would consist of three string elements: "dir1", "dir2", and "file.ext". This
    is encoded as a bencoded list of strings such as l4:dir14:dir28:file.exte

 */

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

  // optional elements
  std::vector<std::string> announceList;
  time_t creationDate;
  std::string comment;
  std::string createdBy;
  std::string encoding;

  // Info dictionary elements
  int64_t pieceLength;
  std::string pieces; // could be array of binary hashes
  bool isprivate;     // optional

  std::vector<FileInfo> files;

  static MetaInfo New(BDict &dict);

  static MetaInfo FromStream(std::istream &in);
};
