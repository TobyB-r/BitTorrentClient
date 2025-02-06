cmake_minimum_required(VERSION 3.10)

project(BitTorrent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(BitTorrent src/main.cpp src/bdecoder.cpp src/metainfo.cpp)

target_link_libraries(BitTorrent ${CURL_LIBRARIES})
target_link_libraries(BitTorrent OpenSSL::SSL OpenSSL::Crypto)

target_include_directories(BitTorrent PUBLIC "${PROJECT_SOURCE_DIR}/include")
