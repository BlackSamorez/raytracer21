#pragma once

#include <string>

#include "geometry/vector.h"

namespace scene {
struct Material {
public:
    std::string name;
    geometry::Vector3D<> ambient_color = {0, 0, 0};
    geometry::Vector3D<> diffuse_color = {0, 0, 0};
    geometry::Vector3D<> specular_color = {0, 0, 0};
    geometry::Vector3D<> intensity = {0, 0, 0};
    double specular_exponent = 0;
    double refraction_index = 0;
    geometry::Vector3D<> albedo = {1, 0, 0};
};
}  // namespace scene
