#pragma once

#include <array>
#include <cmath>

#include "geometry/vector.h"

namespace raytracer {
struct CameraOptions {
    int screen_width;
    int screen_height;
    double fov;
    geometry::Vector3D<> look_from;
    geometry::Vector3D<> look_to;

    CameraOptions(int width, int height, double fov = M_PI / 2,
                  std::array<double, 3> look_from = {0.0, 0.0, 0.0},
                  std::array<double, 3> look_to = {0.0, 0.0, -1.0})
        : screen_width(width),
          screen_height(height),
          fov(fov),
          look_from(look_from),
          look_to(look_to)

    {
    }
};
}  // namespace raytracer
