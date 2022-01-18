# raytracer21

A *c++* raytracer programm

![alt text](https://github.com/BlackSamorez/raytracer21/blob/main/examples/trumpet/example.png?raw=true)

## Code Features

* [*Catch2*](https://github.com/catchorg/Catch2) based [tests](/tests/reader/test_reader.cpp) with [**CMake integration**](/tests/CMakeLists.txt) and [**github CI**](/.github/workflows/cmake.yml)
* Templated *geometry* namespace featuring:
  * *c++20* [**requires** and **concepts**](/src/geometry/vector.h)
* *Wavefront .obj* file [parser](/src/scene/scene.h) for scene construction

## Raytracing Features

* Debug mode: 
  * [Depth render](/tests/debug_mode/scenes/classic_box/depth.png)
  * [Normal' render](/tests/debug_mode/scenes/classic_box/normal.png)
* [Phong reflection model](https://en.wikipedia.org/wiki/Phong_reflection_model)
* [Reflections](), [Refractions]() 
* [Stained glass](https://github.com/BlackSamorez/raytracer21/blob/main/examples/dgap/full.png?raw=true)
* [Skybox](/src/raytracer/skybox.h) cubemap support
