#pragma once

#include <memory>
#include <array>

#include "geometry/vector.h"
#include "geometry/triangle.h"
#include "geometry/sphere.h"
#include "scene/material.h"

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
