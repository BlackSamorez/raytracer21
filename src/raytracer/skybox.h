#pragma once

#include <utility>

#include "image.h"
#include "vector.h"
#include "ray.h"

class Sky {
public:
    enum class StrongestDirection { kFront, kBack, kTop, kBottom, kRight, kLeft };

public:
    Sky() : image_({}) {}

    explicit Sky(std::shared_ptr<Image> image) : image_(std::move(image)) {
    }

public:
    static constexpr StrongestDirection GetStrongestDirection(
        const geometry::Vector3D<>& direction) {
        size_t max_abs = std::max_element(direction.data_.begin(), direction.data_.end(),
                                          [](auto a, auto b) { return fabs(a) < fabs(b); }) -
                         direction.data_.begin();
        switch (max_abs) {
            case 0:
                return (direction[0] > 0) ? StrongestDirection::kFront: StrongestDirection::kBack;
            case 1:
                return (direction[1] > 0) ? StrongestDirection::kTop: StrongestDirection::kBottom;
            case 2:
                return (direction[2] > 0) ? StrongestDirection::kRight: StrongestDirection::kLeft;
            default:
                throw std::runtime_error("Direction guessing gone wrong");
        }
    }

public:
    [[nodiscard]] geometry::Vector3D<> Trace(const geometry::Ray<>& ray) const {
        if (!image_) {
            return {0, 0, 0};
        }

        int block_size = image_->Width() / 4;
        auto cube_v = ray.GetDirection();
        cube_v = cube_v / std::fabs(*std::max_element(cube_v.data_.begin(), cube_v.data_.end(),
                                           [](auto a, auto b) { return fabs(a) < fabs(b); }));
        RGB color{};
        switch (GetStrongestDirection(ray.GetDirection())) {
                int x, y;
            case StrongestDirection::kFront:
                x = (3 * block_size) / 2 + static_cast<int>(block_size * cube_v[2] / 2);
                y = (3 * block_size) / 2 + static_cast<int>(block_size * -cube_v[1] / 2);
                color = image_->GetPixel(y, x);
                break;
            case StrongestDirection::kBack:
                x = (7 * block_size) / 2 + static_cast<int>(block_size * -cube_v[2] / 2);
                y = (3 * block_size) / 2 + static_cast<int>(block_size * -cube_v[1] / 2);
                color = image_->GetPixel(y, x);
                break;
            case StrongestDirection::kTop:
                x = (3 * block_size) / 2 + static_cast<int>(block_size * cube_v[2] / 2);
                y = (1 * block_size) / 2 + static_cast<int>(block_size * cube_v[0] / 2);
                color = image_->GetPixel(y, x);
                break;
            case StrongestDirection::kBottom:
                x = (3 * block_size) / 2 + static_cast<int>(block_size * cube_v[2] / 2);
                y = (5 * block_size) / 2 + static_cast<int>(block_size * -cube_v[0] / 2);
                color = image_->GetPixel(y, x);
                break;
            case StrongestDirection::kRight:
                x = (5 * block_size) / 2 + static_cast<int>(block_size * -cube_v[0] / 2);
                y = (3 * block_size) / 2 + static_cast<int>(block_size * -cube_v[1] / 2);
                color = image_->GetPixel(y, x);
                break;
            case StrongestDirection::kLeft:
                x = (1 * block_size) / 2 + static_cast<int>(block_size * cube_v[0] / 2);
                y = (3 * block_size) / 2 + static_cast<int>(block_size * -cube_v[1] / 2);
                color = image_->GetPixel(y, x);
                break;
        }
        return {static_cast<double>(color.r) / 256, static_cast<double>(color.g) / 256, static_cast<double>(color.b) / 256};
    }

public:
    std::shared_ptr<Image> image_;
};