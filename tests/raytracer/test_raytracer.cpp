#ifndef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
#endif
#include <catch2/catch.hpp>

#ifndef PROGRAM_DIR
#define PROGRAM_DIR "./"
#endif

#include "raytracer/raytracer.cpp"

#include "auxiliary.hpp"
#include "raytracer/camera_options.h"
#include "raytracer/image.h"
#include "raytracer/render_options.h"

TEST_CASE("CornellBox-Original full", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    raytracer::CameraOptions camera_options(500, 500);
    camera_options.look_from = {-0.5, 1.5, 0.98};
    camera_options.look_to = {0.0, 1.0, 0.0};
    raytracer::RenderOptions render_options{4, raytracer::RenderMode::kFull};

    auto render =
        raytracer::Render(dir_path + "scenes/classic_box/CornellBox-Original.obj", camera_options, render_options);
    auto target = raytracer::Image(500, 500);
    target.ReadPng(dir_path + "scenes/classic_box/full.png");

    Compare(render, target);
}

TEST_CASE("Opaque glass", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    raytracer::CameraOptions camera_options(500, 500);
    camera_options.look_from = {-2, 2, -1};
    camera_options.look_to = {0, 0, 0};
    raytracer::RenderOptions render_options{4, raytracer::RenderMode::kFull};

    auto render =
        raytracer::Render(dir_path + "scenes/opaque_glass/OpaqueGlass.obj", camera_options, render_options);
    auto target = raytracer::Image(500, 500);
    target.ReadPng(dir_path + "scenes/opaque_glass/full.png");

    Compare(render, target);
//    render.Write(dir_path + "scenes/opaque_glass/full.png");
}

TEST_CASE("Colored glass", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    raytracer::CameraOptions camera_options(500, 500);
    camera_options.look_from = {-2, 2, -1};
    camera_options.look_to = {0, 0, 0};
    raytracer::RenderOptions render_options{4, raytracer::RenderMode::kFull};

    auto render =
        raytracer::Render(dir_path + "scenes/colored_glass/ColoredGlass.obj", camera_options, render_options);
    auto target = raytracer::Image(500, 500);
    target.ReadPng(dir_path + "scenes/colored_glass/full.png");

    Compare(render, target);
//    render.Write(dir_path + "scenes/colored_glass/full.png");
}

TEST_CASE("Simple stained glass", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    raytracer::CameraOptions camera_options(500, 500);
    camera_options.look_from = {-2, 4, -12};
    camera_options.look_to = {0, -2, -4};
    raytracer::RenderOptions render_options{4, raytracer::RenderMode::kFull};

    auto render =
        raytracer::Render(dir_path + "scenes/simple_stained_glass/SimpleStainedGlass.obj", camera_options, render_options);
    auto target = raytracer::Image(500, 500);
    target.ReadPng(dir_path + "scenes/simple_stained_glass/full.png");

    Compare(render, target);
//    render.Write(dir_path + "scenes/simple_stained_glass/full.png");
}

TEST_CASE("Skybox", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    raytracer::CameraOptions camera_options(500, 500);
    camera_options.fov = 2.5;
    camera_options.look_from = {0, 0, 0};
    camera_options.look_to = {1, 1, 1};
    raytracer::RenderOptions render_options{4, raytracer::RenderMode::kFull};

    auto render =
        raytracer::Render(dir_path + "scenes/skybox/Skybox.obj", camera_options, render_options);
        auto target = raytracer::Image(500, 500);
        target.ReadPng(dir_path + "scenes/skybox/full.png");

    Compare(render, target);
//    render.Write(dir_path + "scenes/skybox/full.png");
}

TEST_CASE("Violin case", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    raytracer::CameraOptions camera_options(500, 500);
    camera_options.look_from = {1.5, 1.5, 1.5};
    camera_options.look_to = {0.5, 0, 0};
    raytracer::RenderOptions render_options{4, raytracer::RenderMode::kFull};

    auto render =
        raytracer::Render(dir_path + "scenes/violin_case/ViolinCase.obj", camera_options, render_options);
    auto target = raytracer::Image(500, 500);
    target.ReadPng(dir_path + "scenes/violin_case/full.png");

    Compare(render, target);
//    render.Write(dir_path + "scenes/violin_case/example.png");
}
