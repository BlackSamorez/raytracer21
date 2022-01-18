#pragma once

#include "geometry/vector.h"
#include "raytracer/camera_options.h"

namespace raytracer {
class RayCaster {
public:
    explicit RayCaster(const CameraOptions& camera_options) {
        screen_height_ = camera_options.screen_height;
        screen_width_ = camera_options.screen_width;

        origin_ = camera_options.look_from;
        backward_unit_ = camera_options.look_from - camera_options.look_to;
        backward_unit_.Normalize();

        right_unit_ = CrossProduct({0, 1, 0}, backward_unit_);

        if (Length(right_unit_) < kEpsilon) {
            right_unit_ = {1, 0, 0};
            up_unit_ = {0, 0, 1};
        } else {
            right_unit_.Normalize();
            up_unit_ = CrossProduct(right_unit_, backward_unit_);
            up_unit_.Normalize();
        }

        double pixel_size = 2 * tan(camera_options.fov / 2) / screen_height_;
        up_unit_ *= pixel_size;
        right_unit_ *= pixel_size;

        if (backward_unit_[1] < 0) {
            //            up_unit_ *= -1;  // I have no fucking idea why
        }
    }

public:
    geometry::Ray<> operator()(int horizontal_pixel_index, int vertical_pixel_index) const {
        auto direction =
            (static_cast<double>((2 * horizontal_pixel_index - screen_width_ + 1)) / 2) *
                right_unit_ +
            (static_cast<double>((2 * vertical_pixel_index - screen_height_ + 1)) / 2) * up_unit_ -
            backward_unit_;
        direction.Normalize();
        return {origin_, direction};
    }

public:
    int screen_height_;
    int screen_width_;

private:
    constexpr static double kEpsilon = 0.0001;

private:
    geometry::Vector3D<> origin_;
    geometry::Vector3D<> backward_unit_;

    geometry::Vector3D<> right_unit_;
    geometry::Vector3D<> up_unit_;
};
}  // namespace raytracer
