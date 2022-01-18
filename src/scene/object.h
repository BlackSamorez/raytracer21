#pragma once

#include <triangle.h>
#include <material.h>
#include <sphere.h>

#include <memory>

namespace scene {
struct Object {
public:
    [[nodiscard]] geometry::Vector3D<>*& GetNormal(size_t index) {
        return normals[index];
    }

public:
    const Material* material = nullptr;
    geometry::Triangle<> polygon;
    std::array<geometry::Vector3D<>*, 3> normals = {nullptr, nullptr, nullptr};
};
struct SphereObject {
public:
    const Material* material = nullptr;
    geometry::Sphere<> sphere;
};
}  // namespace scene
