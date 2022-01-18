#pragma once

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>

#include "geometry/vector.h"
#include "scene/scene.h"
#include "scene/material.h"
#include "scene/object.h"
#include "scene/light.h"
#include "scene/skybox.h"
#include "raytracer/image.h"

namespace {
inline geometry::Vector3D<> GetThreeNumbers(const std::vector<std::string>& attributes,
                                            int begin = 1) {
    return {std::stod(attributes[begin]), std::stod(attributes[begin + 1]),
            std::stod(attributes[begin + 2])};
}

inline std::vector<std::string> ParseLine(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> parsed_line;

    for (std::string s; iss >> s;) {
        parsed_line.push_back(s);
    }
    return parsed_line;
}

inline std::pair<int, int> ParsePointTriplet(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> parsed_line;
    std::string s;

    while (std::getline(iss, s, '/')) {
        if (!s.empty()) {
            parsed_line.push_back(s);
        } else {
            parsed_line.emplace_back("0");
        }
    }
    if (parsed_line.size() < 3) {
        return {std::stod(parsed_line[0]), 0};  // zero means no normal
    }

    return {std::stod(parsed_line[0]), std::stod(parsed_line[2])};
}

inline std::string GetFolderPathFromFilePath(const std::string& s) {
    char sep = '/';

    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return s.substr(0, i);
    }
    return "";
}
}  // namespace

namespace scene {
std::map<std::string, std::unique_ptr<scene::Material>> ConstructMaterials(std::istream& input) {
    std::map<std::string, std::unique_ptr<Material>> materials;

    std::string line;
    bool inside_material = false;
    Material current_material;

    while (std::getline(input, line)) {
        std::vector<std::string> attributes = ParseLine(line);
        if (attributes.empty()) {
            continue;
        }

        if (attributes[0] == "newmtl") {
            if (inside_material) {
                materials[current_material.name] = std::make_unique<Material>(current_material);
            }
            current_material = Material{};
            inside_material = true;
            current_material.name = attributes[1];
        }

        if (attributes[0] == "Ka") {
            current_material.ambient_color = GetThreeNumbers(attributes);
        }

        if (attributes[0] == "Kd") {
            current_material.diffuse_color = GetThreeNumbers(attributes);
        }

        if (attributes[0] == "Ks") {
            current_material.specular_color = GetThreeNumbers(attributes);
        }

        if (attributes[0] == "Ke") {
            current_material.intensity = GetThreeNumbers(attributes);
        }

        if (attributes[0] == "Ns") {
            current_material.specular_exponent = std::stod(attributes[1]);
        }

        if (attributes[0] == "Ni") {
            current_material.refraction_index = std::stod(attributes[1]);
        }

        if (attributes[0] == "al") {
            current_material.albedo = GetThreeNumbers(attributes);
        }
    }

    materials[current_material.name] = std::make_unique<Material>(current_material);

    return materials;
}
std::map<std::string, std::unique_ptr<scene::Material>> ReadMaterials(std::string_view filename) {
    std::ifstream infile(static_cast<std::string>(filename));
    return ConstructMaterials(infile);
}
Scene ConstructScene(std::istream& input, const std::string& path) {
    // Material fields
    std::vector<Object> objects;
    std::vector<SphereObject> sphere_objects;
    std::vector<Light> lights;
    Sky sky;
    std::map<std::string, std::unique_ptr<scene::Material>> materials_pointers;
    std::vector<std::unique_ptr<geometry::Vector3D<>>> normal_pointers;

    // Aux objects
    std::vector<geometry::Vector3D<>> vertices;
    std::string line;
    Material* current_material = nullptr;

    while (std::getline(input, line)) {
        std::vector<std::string> attributes = ParseLine(line);
        if (attributes.empty()) {
            continue;
        }

        if (attributes[0] == "v") {
            vertices.push_back(GetThreeNumbers(attributes));
        }

        if (attributes[0] == "vt") {
            continue;
        }

        if (attributes[0] == "vn") {
            normal_pointers.push_back(
                std::make_unique<geometry::Vector3D<>>(GetThreeNumbers(attributes)));
            continue;
        }

        if (attributes[0] == "f") {
            size_t number_of_vertices = attributes.size() - 1;
            for (size_t i = 0; i < number_of_vertices - 2; ++i) {
                std::vector<std::pair<int, int>> indices = {ParsePointTriplet(attributes[1]),
                                                            ParsePointTriplet(attributes[i + 2]),
                                                            ParsePointTriplet(attributes[i + 3])};

                std::vector<geometry::Vector3D<>*> normals(3, nullptr);
                for (int j = 0; j < 3; ++j) {
                    if (indices[j].first < 1) {
                        indices[j].first = static_cast<int>(vertices.size()) + indices[j].first;
                    } else {
                        indices[j].first--;
                    }
                    if (indices[j].second == 0) {  // no normal
                        normals[j] = nullptr;
                    } else {
                        if (indices[j].second < 1) {
                            normals[j] =
                                normal_pointers[normal_pointers.size() + indices[j].second].get();
                        } else {
                            normals[j] = normal_pointers[indices[j].second - 1].get();
                        }
                    }
                }

                Object object{
                    current_material,
                    geometry::Triangle{vertices[indices[0].first], vertices[indices[1].first],
                                       vertices[indices[2].first]},
                    {normals[0], normals[1], normals[2]}};
                objects.push_back(object);
            }
            continue;
        }

        if (attributes[0] == "mtllib") {
            materials_pointers = ReadMaterials(path + "/" + attributes[1]);
            continue;
        }

        if (attributes[0] == "usemtl") {
            current_material = materials_pointers.at(attributes[1]).get();
            continue;
        }

        if (attributes[0] == "S") {
            sphere_objects.push_back(
                {current_material,
                 geometry::Sphere{GetThreeNumbers(attributes), std::stod(attributes[4])}});
            continue;
        }

        if (attributes[0] == "P") {
            lights.push_back({GetThreeNumbers(attributes), GetThreeNumbers(attributes, 4)});
            continue;
        }

        if (attributes[0] == "Sky") {
            sky.image_ = std::make_shared<raytracer::Image>(path + "/" + attributes[3]);
        }
    }

    return {objects,
            sphere_objects,
            lights,
            std::move(sky),
            std::move(materials_pointers),
            std::move(normal_pointers)};
}
Scene ReadScene(std::string_view filename) {
    std::ifstream infile(static_cast<std::string>(filename));
    return ConstructScene(infile, GetFolderPathFromFilePath(static_cast<std::string>(filename)));
}
}  // namespace scene
