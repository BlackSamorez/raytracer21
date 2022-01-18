#pragma once

#include "geometry/vector.h"

namespace scene {
struct Light {
    geometry::Vector3D<> position;
    geometry::Vector3D<> intensity;
};
}  // namespace scene
