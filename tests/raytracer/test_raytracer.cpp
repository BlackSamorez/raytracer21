#ifndef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
#endif
#include <catch2/catch.hpp>

#ifndef PROGRAM_DIR
#define PROGRAM_DIR "./"
#endif

#include "raytracer.h"
#include "auxiliary.hpp"

TEST_CASE("CornellBox-Original full", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    CameraOptions camera_options(500, 500);
    camera_options.look_from = {-0.5, 1.5, 0.98};
    camera_options.look_to = {0.0, 1.0, 0.0};
    RenderOptions render_options{4, RenderMode::kFull};

    auto render =
        Render(dir_path + "scenes/classic_box/CornellBox-Original.obj", camera_options, render_options);
    auto target = Image(500, 500);
    target.ReadPng(dir_path + "scenes/classic_box/full.png");

    Compare(render, target);
}

TEST_CASE("Opaque glass", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    CameraOptions camera_options(500, 500);
    camera_options.look_from = {-2, 2, -1};
    camera_options.look_to = {0, 0, 0};
    RenderOptions render_options{4, RenderMode::kFull};

    auto render =
        Render(dir_path + "scenes/opaque_glass/OpaqueGlass.obj", camera_options, render_options);
    auto target = Image(500, 500);
    target.ReadPng(dir_path + "scenes/opaque_glass/full.png");

    Compare(render, target);
//    render.Write(dir_path + "scenes/opaque_glass/full.png");
}

TEST_CASE("Colored glass", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    CameraOptions camera_options(500, 500);
    camera_options.look_from = {-2, 2, -1};
    camera_options.look_to = {0, 0, 0};
    RenderOptions render_options{4, RenderMode::kFull};

    auto render =
        Render(dir_path + "scenes/colored_glass/ColoredGlass.obj", camera_options, render_options);
    auto target = Image(500, 500);
    target.ReadPng(dir_path + "scenes/colored_glass/full.png");

    Compare(render, target);
//    render.Write(dir_path + "scenes/colored_glass/full.png");
}

TEST_CASE("Simple stained glass", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    CameraOptions camera_options(500, 500);
    camera_options.look_from = {-2, 4, -12};
    camera_options.look_to = {0, -2, -4};
    RenderOptions render_options{4, RenderMode::kFull};

    auto render =
        Render(dir_path + "scenes/simple_stained_glass/SimpleStainedGlass.obj", camera_options, render_options);
    auto target = Image(500, 500);
    target.ReadPng(dir_path + "scenes/simple_stained_glass/full.png");

    Compare(render, target);
//    render.Write(dir_path + "scenes/simple_stained_glass/full.png");
}

TEST_CASE("Skybox", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    CameraOptions camera_options(500, 500);
    camera_options.fov = 2.5;
    camera_options.look_from = {0, 0, 0};
    camera_options.look_to = {1, 1, 1};
    RenderOptions render_options{4, RenderMode::kFull};

    auto render =
        Render(dir_path + "scenes/skybox/Skybox.obj", camera_options, render_options);
        auto target = Image(500, 500);
        target.ReadPng(dir_path + "scenes/skybox/full.png");

    Compare(render, target);
//    render.Write(dir_path + "scenes/skybox/full.png");
}
