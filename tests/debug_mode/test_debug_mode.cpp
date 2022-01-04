#ifndef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
#endif
#include <catch2/catch.hpp>

#ifndef PROGRAM_DIR
#define PROGRAM_DIR "./"
#endif

#include "raytracer.h"

TEST_CASE("Depth temp", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);

    CameraOptions camera_options(500, 500);
    camera_options.look_from = {-0.5, 1.5, 0.98};
    camera_options.look_to = {0.0, 1.0, 0.0};
    RenderOptions render_options{4, RenderMode::kDepth};

    auto image = Render(dir_path+ "classic_box/CornellBox-Original.obj", camera_options, render_options);
    image.Write(dir_path+ "classic_box/default.png");
}