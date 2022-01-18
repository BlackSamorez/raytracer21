#pragma once

#include <vector>
#include <memory>
#include <map>

#include "scene/material.h"
#include "scene/object.h"
#include "scene/light.h"
#include "scene/skybox.h"

typedef std::map<std::string, std::unique_ptr<scene::Material>> MaterialPointers;

namespace scene {
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

public:
    [[nodiscard]] static std::map<std::string, Material> BuildMaterialsFromPointers(
        const MaterialPointers& pointers) {
        static std::map<std::string, Material> materials;
        if (materials.size() != pointers.size()) {  // None of them are supposed to change
            for (const auto& [name, pointer] : pointers) {
                materials[name] = *pointer;
            }
        }
        return materials;
    }

public:
    const std::vector<Object> objects_;
    const std::vector<SphereObject> sphere_objects_;
    const std::vector<Light> lights_;
    const Sky sky_;

public:  // heap held
    const MaterialPointers materials_pointers_;
    const std::vector<std::unique_ptr<geometry::Vector3D<>>> normals_;
};
}  // namespace scene
