cmake_minimum_required(VERSION 3.30)
project(xyTools2)


if (MSVC)
    add_compile_options("/source-charset:utf-8" "/execution-charset:utf-8")
endif ()

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Third/NESupport)
add_subdirectory(Third/stb)
add_subdirectory(Third/nlohmann_json)
add_subdirectory(Third/xy2)

add_executable(xyTools2 main.cpp
        utils.cpp
        utils.h)

target_link_libraries(xyTools2 PUBLIC nesupport stb nlohmann_json XY2)
