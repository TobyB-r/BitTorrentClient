#include "metainfo.h"
#include "bdecoder.h"
#include "globals.h"
#include <cstdint>
#include <cstring>
#include <curl/curl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
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

  auto pieceStr = std::get<std::string>(infoDict.map["pieces"].inner);

  for (int i = 0; i < pieceStr.length(); i += 20) {
    sha1_hash_t hash;

    std::copy(pieceStr.begin() + i, pieceStr.begin() + i + 20, hash.begin());
    
    metaInfo.pieces.push_back(hash);
  }

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

  auto metainfo = MetaInfo::New(dict);

  in.seekg(dict.map["info"].pos);

  auto infoString = std::string(dict.map["info"].length, '\0');
  in.read(&infoString[0], dict.map["info"].length);

  // hashing info dictionary
  int fail = 1;

  if (!EVP_DigestInit_ex(context, sha1, NULL))
    goto err;

  if (!EVP_DigestUpdate(context, infoString.data(), infoString.length()))
    goto err;

  unsigned int len;

  if (!EVP_DigestFinal_ex(context, metainfo.infoHash.data(), &len))
    goto err;

  fail = 0;

err:
  if (fail) {
    std::cerr << "Error hashing info dictionary" << std::endl;
    ERR_print_errors_fp(stderr);
    exit(1);
  }

  return metainfo;
}

FileInfo::FileInfo(int64_t length, std::string path)
    : length(length), path(path) {}

// callback for curl to write the data it recieves
size_t write_fun(void *ptr, size_t size, size_t nmemb, void *data) {
  size_t realsize = size * nmemb;
  std::string *str = (std::string *)data;
  str->append((char *)ptr, realsize);
  return realsize;
}

// Makes http get request to tracker to announce our joining
// see
// https://wiki.theory.org/BitTorrentSpecification#Tracker_HTTP.2FHTTPS_Protocol
std::string announce(MetaInfo &info) {
  CURL *curl = curl_easy_init();
  std::string data;
  std::string url = info.announce;

  url += "?info_hash=";
  url += "&param2=";

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_fun);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);

    std::cout << "curling" << std::endl;

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      std::cerr << "libcurl error" << std::endl;
      std::cerr << curl_easy_strerror(res);
      exit(res);
    }

    std::cout << info.announce << std::endl << data << std::endl;

    curl_easy_cleanup(curl);
  } else {
    std::cerr << "Error setting up libcurl" << std::endl;
    exit(1);
  }

  return data;
}
