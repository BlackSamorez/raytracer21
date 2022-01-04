# raytracer21

A *c++* raytracer programm

## Features

* [*Catch2*](https://github.com/catchorg/Catch2) based [tests](/tests/reader/test_reader.cpp) with [**CMake integration**](/tests/CMakeLists.txt) and [**github CI**](/.github/workflows/cmake.yml)
* Templated *geometry* namespace featuring:
  * *c++20* [**requires** and **concepts**](/src/geometry/vector.h)
* *Wavefront .obj* file [parser](/src/reader/scene.h) for scene construction
