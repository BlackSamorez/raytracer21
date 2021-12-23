#pragma once

#include <triangle.h>
#include <material.h>
#include <sphere.h>

struct Object {
    const Material *material = nullptr;
    Triangle polygon;
    const std::vector<Vector *> normals;

    const Vector *GetNormal(size_t index) const {
        if (normals.empty()) {
            return nullptr;
        } else {
            return normals[index];
        }
    }

    const Triangle &GetPolygon() const {
        return polygon;
    }
};

struct SphereObject {
    const Material *material = nullptr;
    Sphere sphere;
};
