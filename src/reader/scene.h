#pragma once

#include <material.h>
#include <vector.h>
#include <object.h>
#include <light.h>

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>

std::vector<std::string> ParseLine(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> parsed_line;

    for (std::string s; iss >> s;) {
        parsed_line.push_back(s);
    }
    return parsed_line;
}

std::pair<int, int> ParsePointTriplet(const std::string& line) {
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

std::string GetFolderPathFromFilePath(const std::string& s) {
    char sep = '/';

    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return s.substr(0, i);
    }
    return "";
}

class Scene {
public:
    [[nodiscard]] const std::vector<Object>& GetObjects() const {
        return objects_;
    }

    [[nodiscard]] const std::vector<SphereObject>& GetSphereObjects() const {
        return sphere_objects_;
    }

    [[nodiscard]] const std::vector<Light>& GetLights() const {
        return lights_;
    }

    [[nodiscard]] const std::map<std::string, std::unique_ptr<Material>>& GetMaterials() const {
        return materials_pointers_;
    }

public:
    void BuildMaterialsFromPointers() {
        for (const auto& [name, pointer] : materials_pointers_) {
            materials_[name] = *pointer;
        }
    }

private:
    std::vector<Object> objects_;
    std::vector<SphereObject> sphere_objects_;
    std::vector<Light> lights_;
    std::map<std::string, std::unique_ptr<Material>> materials_pointers_;
    std::map<std::string, Material> materials_;
    std::vector<std::unique_ptr<geometry::Vector3D<>>> normals_;

private:
    friend inline Scene ReadScene(std::string_view filename);
};

inline std::map<std::string, std::unique_ptr<Material>> ReadMaterials(std::string_view filename) {
    std::map<std::string, std::unique_ptr<Material>> materials;

    std::ifstream infile(static_cast<std::string>(filename));
    std::string line;

    bool inside_material = false;
    Material current_material;
    while (std::getline(infile, line)) {
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
            current_material.ambient_color = {std::stod(attributes[1]), std::stod(attributes[2]),
                                              std::stod(attributes[3])};
        }

        if (attributes[0] == "Kd") {
            current_material.diffuse_color = {std::stod(attributes[1]), std::stod(attributes[2]),
                                              std::stod(attributes[3])};
        }

        if (attributes[0] == "Ks") {
            current_material.specular_color = {std::stod(attributes[1]), std::stod(attributes[2]),
                                               std::stod(attributes[3])};
        }

        if (attributes[0] == "Ke") {
            current_material.intensity = {std::stod(attributes[1]), std::stod(attributes[2]),
                                          std::stod(attributes[3])};
        }

        if (attributes[0] == "Ns") {
            current_material.specular_exponent = std::stod(attributes[1]);
        }

        if (attributes[0] == "Ni") {
            current_material.refraction_index = std::stod(attributes[1]);
        }

        if (attributes[0] == "al") {
            current_material.albedo = {std::stod(attributes[1]), std::stod(attributes[2]),
                                       std::stod(attributes[3])};
        }
    }

    materials[current_material.name] = std::make_unique<Material>(current_material);

    return materials;
}

Scene ReadScene(std::string_view filename) {
    Scene scene;
    std::vector<geometry::Vector3D<>> vertices;

    std::ifstream infile(static_cast<std::string>(filename));
    std::string line;

    Material* current_material = nullptr;
    while (std::getline(infile, line)) {
        std::vector<std::string> attributes = ParseLine(line);
        if (attributes.empty()) {
            continue;
        }

        if (attributes[0] == "v") {
            vertices.push_back(geometry::Vector3D{
                std::stod(attributes[1]), std::stod(attributes[2]), std::stod(attributes[3])});
        }

        if (attributes[0] == "vt") {
            continue;
        }

        if (attributes[0] == "vn") {
            auto normal = geometry::Vector3D{std::stod(attributes[1]), std::stod(attributes[2]),
                                             std::stod(attributes[3])};
            scene.normals_.push_back(std::make_unique<geometry::Vector3D<>>(normal));
        }

        if (attributes[0] == "f") {
            size_t number_of_vertices = attributes.size() - 1;
            for (size_t i = 0; i < number_of_vertices - 2; ++i) {
                std::vector<std::pair<int, int>> indices;
                indices.push_back(ParsePointTriplet(attributes[1]));
                indices.push_back(ParsePointTriplet(attributes[i + 2]));
                indices.push_back(ParsePointTriplet(attributes[i + 3]));

                std::vector<geometry::Vector3D<>*> normals;
                for (int j = 0; j < 3; ++j) {
                    if (indices[j].first < 1) {
                        indices[j].first = static_cast<int>(vertices.size()) + indices[j].first;
                    } else {
                        indices[j].first--;
                    }
                    if (indices[j].second == 0) {  // no normal
                        normals.push_back(nullptr);
                    } else {
                        if (indices[j].second < 1) {
                            normals.push_back(
                                scene.normals_[scene.normals_.size() + indices[j].second].get());
                        } else {
                            normals.push_back(scene.normals_[indices[j].second - 1].get());
                        }
                    }
                }

                Object object{
                    current_material,
                    geometry::Triangle{vertices[indices[0].first], vertices[indices[1].first],
                                       vertices[indices[2].first]},
                    normals};
                scene.objects_.push_back(object);
            }
        }

        if (attributes[0] == "mtllib") {
            scene.materials_pointers_ =
                ReadMaterials(GetFolderPathFromFilePath(static_cast<std::string>(filename)) + "/" +
                              attributes[1]);
        }

        if (attributes[0] == "usemtl") {
            current_material = scene.materials_pointers_.at(attributes[1]).get();
        }

        if (attributes[0] == "S") {
            SphereObject sphere_object = {
                current_material, geometry::Sphere{geometry::Vector3D{std::stod(attributes[1]),
                                                                      std::stod(attributes[2]),
                                                                      std::stod(attributes[3])},
                                                   std::stod(attributes[4])}};
            scene.sphere_objects_.push_back(sphere_object);
        }

        if (attributes[0] == "P") {
            scene.lights_.push_back(
                Light{geometry::Vector3D{std::stod(attributes[1]), std::stod(attributes[2]),
                                         std::stod(attributes[3])},
                      geometry::Vector3D{std::stod(attributes[4]), std::stod(attributes[5]),
                                         std::stod(attributes[6])}});
        }
    }

    scene.BuildMaterialsFromPointers();
    return scene;
}
