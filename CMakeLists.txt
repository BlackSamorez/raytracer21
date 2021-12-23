cmake_minimum_required(VERSION 3.16)
project(raytracer21)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_ASAN "-g -fsanitize=address,undefined -fno-sanitize-recover=all"
        CACHE STRING "Compiler flags in asan build"
        FORCE)

include_directories(src/geometry)
include_directories(src/raytracer)
include_directories(src/reader)

add_subdirectory(tests)