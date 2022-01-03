#pragma once

#include <triangle.h>
#include <material.h>
#include <sphere.h>

#include <memory>

struct Object {
public:
    [[nodiscard]] geometry::Vector3D<>*& GetNormal(size_t index) {
        return normals[index];
    }

public:
    const Material* material = nullptr;
    geometry::Triangle<> polygon;
    std::vector<geometry::Vector3D<>*> normals;
};

struct SphereObject {
public:
    const Material* material = nullptr;
    geometry::Sphere<> sphere;
};
