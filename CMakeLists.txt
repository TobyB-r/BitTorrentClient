cmake_minimum_required(VERSION 3.10)

project(BitTorrent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(BitTorrent src/main.cpp src/bdecoder.cpp src/metainfo.cpp)

target_include_directories(BitTorrent PUBLIC "${PROJECT_SOURCE_DIR}/include")
