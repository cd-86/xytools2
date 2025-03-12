cmake_minimum_required(VERSION 3.10)
project(XY2 VERSION 1.0.0 LANGUAGES CXX)

add_library(XY2 STATIC mapx.cpp ujpeg.cpp Sprite.cpp wdf.cpp)

target_include_directories(XY2 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
