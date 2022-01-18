#ifndef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
#endif
#include <catch2/catch.hpp>

#include <string>

#include "scene/reader.cpp"

#ifndef PROGRAM_DIR
#define PROGRAM_DIR "./"
#endif


TEST_CASE("Read without errors", "[raytracer]") {
    const std::string dir_path(PROGRAM_DIR);
    auto scene = scene::ReadScene(dir_path+ "classic_box/CornellBox-Original.obj");
}

TEST_CASE("Materials read correctly") {
    const std::string dir_path(PROGRAM_DIR);
    auto scene = scene::ReadScene(dir_path+ "classic_box/CornellBox-Original.obj");
    const auto& materials = scene::Scene::BuildMaterialsFromPointers(scene.materials_pointers_);

    SECTION("Number of materials") {
        REQUIRE(materials.size() == 8);
    }


    SECTION("leftWall material read correctly") {
        REQUIRE(materials.contains("leftWall"));

        const auto& left_wall = materials.at("leftWall");
        REQUIRE(left_wall.specular_exponent == 10);

        REQUIRE(left_wall.ambient_color[0] == 0.063);
        REQUIRE(left_wall.ambient_color[1] == 0.0065);
        REQUIRE(left_wall.ambient_color[2] == 0.005);
    }
}

TEST_CASE("Objects read correctly") {
    const std::string dir_path(PROGRAM_DIR);
    auto scene = scene::ReadScene(dir_path+ "classic_box/CornellBox-Original.obj");
    const auto& objects = scene.GetObjects();

    SECTION("Number of objects") {
        REQUIRE(objects.size() == 36);
    }
}
