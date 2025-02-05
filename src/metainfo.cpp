#include "metainfo.h"
#include "bdecoder.h"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

// the metainfo is the bencoded information in the .torrent file
// see https://wiki.theory.org/BitTorrentSpecification#Metainfo_File_Structure

MetaInfo MetaInfo::New(BDict &dict) {
  std::unordered_map<std::string, BObj> &map = dict.map;

  BDict &infoDict = std::get<BDict>(dict.map["info"].inner);
  MetaInfo metaInfo{};

  if (infoDict.map.contains("files")) { // multi file mode
    metaInfo.singleFile = false;
    auto &filesList = std::get<std::vector<BObj>>(infoDict.map["files"].inner);

    for (auto &fileObj : filesList) {
      auto &fileDict = std::get<BDict>(fileObj.inner);

      std::string s;

      auto vec = std::get<std::vector<BObj>>(fileDict.map["path"].inner);

      for (auto &x : vec) {
        s += std::get<std::string>(x.inner);
      }

      metaInfo.files.push_back(
          FileInfo(std::get<int64_t>(fileDict.map["length"].inner), s));
    }
  } else { // single file mode
    metaInfo.singleFile = true;
    metaInfo.files.push_back(
        FileInfo(std::get<int64_t>(infoDict.map["length"].inner), ""));
  }

  metaInfo.name = std::get<std::string>(infoDict.map["name"].inner);
  metaInfo.announce = std::get<std::string>(dict.map["announce"].inner);

  metaInfo.pieces = std::get<std::string>(infoDict.map["pieces"].inner);
  metaInfo.pieceLength = std::get<int64_t>(infoDict.map["piece length"].inner);

  // optional entries
  if (dict.map.contains("comment"))
    metaInfo.comment = std::get<std::string>(dict.map["comment"].inner);

  if (dict.map.contains("created by"))
    metaInfo.createdBy = std::get<std::string>(dict.map["created by"].inner);

  if (dict.map.contains("creation date"))
    metaInfo.creationDate = std::get<int64_t>(dict.map["creation date"].inner);

  return metaInfo;
}

MetaInfo MetaInfo::FromStream(std::istream &in) {
  BObj obj = decodeObject(in);

  if (!std::holds_alternative<BDict>(obj.inner)) {
    throw std::runtime_error("MetaInfo is not formatted as a dictionary");
  }

  BDict &dict = std::get<BDict>(obj.inner);

  auto metainfo =  MetaInfo::New(dict);

  in.seekg(dict.map["info"].pos);
  
  metainfo.infoString = std::string(dict.map["info"].length, '\0');
  in.read(&metainfo.infoString[0], dict.map["info"].length);
  
  return metainfo;
}

FileInfo::FileInfo(int64_t length, std::string path)
    : length(length), path(path) {}
