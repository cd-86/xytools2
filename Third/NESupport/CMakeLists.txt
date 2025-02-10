cmake_minimum_required(VERSION 3.10)
project(NESupport VERSION 1.0.0 LANGUAGES CXX)

add_library(nesupport STATIC NESupport.cpp NESupport.h)

target_include_directories(nesupport PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

