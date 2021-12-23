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

std::vector<std::string> ParseLine(std::string line) {
    std::istringstream iss(line);
    std::vector<std::string> parsed_line;

    for (std::string s; iss >> s;) {
        parsed_line.push_back(s);
    }
    return parsed_line;
}

std::pair<int, int> ParseAttribute(std::string line) {
    std::istringstream iss(line);
    std::vector<std::string> parsed_line;
    std::string s;

    while (std::getline(iss, s, '/')) {
        if (!s.empty()) {
            parsed_line.push_back(s);
        } else {
            parsed_line.push_back("0");
        }
    }
    if (parsed_line.size() < 3) {
        return std::make_pair(std::stod(parsed_line[0]), 0);
    }

    return std::make_pair(std::stod(parsed_line[0]), std::stod(parsed_line[2]));
}

std::string GetFolderPathFromFilePath(std::string s) {
    char sep = '/';

    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return (s.substr(0, i));
    }
    return "";
}

class Scene {
    std::vector<Object> objects_;
    std::vector<SphereObject> sphere_objects_;
    std::vector<Light> lights_;
    std::map<std::string, Material*> material_pointers_;
    std::map<std::string, Material> materials_;
    std::vector<Vector*> normals_;

public:
    const std::vector<Object>& GetObjects() const {
        return objects_;
    }

    const std::vector<SphereObject>& GetSphereObjects() const {
        return sphere_objects_;
    }

    const std::vector<Light>& GetLights() const {
        return lights_;
    }

    const std::map<std::string, Material>& GetMaterials() const {
        return materials_;
    }

    void BuildMaterialsFromPointers() {
        materials_.clear();
        for (auto [key, value] : material_pointers_) {
            materials_[key] = *value;
        }
    }

    virtual ~Scene() {
        for (auto [key, pointer] : material_pointers_) {
            delete pointer;
        }
    }

    friend inline Scene ReadScene(std::string_view filename);
};

inline std::map<std::string, Material*> ReadMaterials(std::string_view filename) {
    std::map<std::string, Material*> materials;

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
                Material* newly_placed_material = new Material();
                *newly_placed_material = current_material;
                materials[newly_placed_material->name] = newly_placed_material;
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
    Material* newly_placed_material = new Material();
    *newly_placed_material = current_material;
    materials[newly_placed_material->name] = newly_placed_material;

    return materials;
}

inline Scene ReadScene(std::string_view filename) {
    Scene scene;
    std::vector<Vector> vertices;

    std::ifstream infile(static_cast<std::string>(filename));
    std::string line;

    Material* current_material = nullptr;
    while (std::getline(infile, line)) {
        std::vector<std::string> attributes = ParseLine(line);
        if (attributes.empty()) {
            continue;
        }

        if (attributes[0] == "v") {
            vertices.push_back(Vector{std::stod(attributes[1]), std::stod(attributes[2]),
                                      std::stod(attributes[3])});
        }

        if (attributes[0] == "vt") {
            continue;
        }

        if (attributes[0] == "vn") {
            Vector* normal = new Vector{std::stod(attributes[1]), std::stod(attributes[2]),
                                        std::stod(attributes[3])};
            scene.normals_.push_back(normal);
        }

        if (attributes[0] == "f") {
            size_t number_of_vertices = attributes.size() - 1;
            for (size_t i = 0; i < number_of_vertices - 2; ++i) {
                std::vector<std::pair<int, int>> indices;
                indices.push_back(ParseAttribute(attributes[1]));
                indices.push_back(ParseAttribute(attributes[i + 2]));
                indices.push_back(ParseAttribute(attributes[i + 3]));

                std::vector<Vector*> normals;
                for (int j = 0; j < 3; ++j) {
                    if (indices[j].first < 1) {
                        indices[j].first = vertices.size() + indices[j].first;
                    } else {
                        indices[j].first--;
                    }
                    if (indices[j].second == 0) {
                        normals.push_back(nullptr);
                    } else {
                        if (indices[j].second < 1) {
                            normals.push_back(
                                scene.normals_[scene.normals_.size() + indices[j].second]);
                        } else {
                            normals.push_back(scene.normals_[indices[j].second - 1]);
                        }
                    }
                }

                Object object = {current_material,
                                 Triangle{vertices[indices[0].first], vertices[indices[1].first],
                                          vertices[indices[2].first]},
                                 normals};
                scene.objects_.push_back(object);
            }
        }

        if (attributes[0] == "mtllib") {
            scene.material_pointers_ =
                ReadMaterials(GetFolderPathFromFilePath(static_cast<std::string>(filename)) + "/" +
                              attributes[1]);
        }

        if (attributes[0] == "usemtl") {
            current_material = scene.material_pointers_.at(attributes[1]);
        }

        if (attributes[0] == "S") {
            SphereObject sphere_object = {
                current_material, Sphere{Vector{std::stod(attributes[1]), std::stod(attributes[2]),
                                                std::stod(attributes[3])},
                                         std::stod(attributes[4])}};
            scene.sphere_objects_.push_back(sphere_object);
        }

        if (attributes[0] == "P") {
            scene.lights_.push_back(Light{Vector{std::stod(attributes[1]), std::stod(attributes[2]),
                                                 std::stod(attributes[3])},
                                          Vector{std::stod(attributes[4]), std::stod(attributes[5]),
                                                 std::stod(attributes[6])}});
        }
    }

    scene.BuildMaterialsFromPointers();
    return scene;
}
