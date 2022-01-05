cmake_minimum_required(VERSION 3.16)
project(raytracer21)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")

SET(CMAKE_CXX_FLAGS  "-fconcepts")

set(CMAKE_CXX_FLAGS_ASAN "-g -fsanitize=address,undefined -fno-sanitize-recover=all -fconcepts"
        CACHE STRING "Compiler flags in asan build"
        FORCE)

include_directories(src/geometry)
include_directories(src/raytracer)
include_directories(src/reader)

enable_testing()
add_subdirectory(tests)