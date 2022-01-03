#pragma once

#include <vector.h>
#include <triangle.h>

struct Light {
    geometry::Vector3D<> position;
    geometry::Vector3D<> intensity;
};
