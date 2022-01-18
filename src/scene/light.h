#pragma once

#include <vector.h>
#include <triangle.h>

namespace scene {
struct Light {
    geometry::Vector3D<> position;
    geometry::Vector3D<> intensity;
};
}  // namespace scene
