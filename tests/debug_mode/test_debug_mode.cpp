#ifndef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
#endif
#include <catch2/catch.hpp>

#ifndef PROGRAM_DIR
#define PROGRAM_DIR "./"
#endif

#include "raytracer.cpp"
#include "auxiliary.hpp"
#include "camera_options.h"
#include "image.h"
#include "render_options.h"

TEST_CASE("CornellBox-Original depth", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    raytracer::CameraOptions camera_options(500, 500);
    camera_options.look_from = {-0.5, 1.5, 0.98};
    camera_options.look_to = {0.0, 1.0, 0.0};
    raytracer::RenderOptions render_options{4, raytracer::RenderMode::kDepth};

    auto render =
        raytracer::Render(dir_path + "scenes/classic_box/CornellBox-Original.obj", camera_options, render_options);
    auto target = raytracer::Image(500, 500);
    target.ReadPng(dir_path + "scenes/classic_box/depth.png");

    Compare(render, target);
}

TEST_CASE("CornellBox-Original normal", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    raytracer::CameraOptions camera_options(500, 500);
    camera_options.look_from = {-0.5, 1.5, 0.98};
    camera_options.look_to = {0.0, 1.0, 0.0};
    raytracer::RenderOptions render_options{4, raytracer::RenderMode::kNormal};

    auto render =
        raytracer::Render(dir_path + "scenes/classic_box/CornellBox-Original.obj", camera_options, render_options);
    auto target = raytracer::Image(500, 500);
    target.ReadPng(dir_path + "scenes/classic_box/normal.png");

    Compare(render, target);
}