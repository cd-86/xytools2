project(nlohmann_json)

file(GLOB SRCS *.cpp)

add_library(nlohmann_json STATIC ${SRCS})

target_include_directories(nlohmann_json PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)