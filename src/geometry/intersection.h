#pragma once

#include <parameters.h>
#include <vector.h>

namespace geometry {
template <typename VectorNumericType = DefaultNumericType>
requires std::floating_point<VectorNumericType>
class Intersection {
public:
    Intersection(Vector3D<VectorNumericType> pos, Vector3D<VectorNumericType> norm, double dist)
        : position_(pos), normal_(norm), distance_(dist) {
    }

public:
    const Vector3D<VectorNumericType>& GetPosition() const {
        return position_;
    }

    const Vector3D<VectorNumericType>& GetNormal() const {
        return normal_;
    }

    VectorNumericType GetDistance() const {
        return distance_;
    }

private:
    Vector3D<VectorNumericType> position_;
    Vector3D<VectorNumericType> normal_;
    double distance_;
};
}  // namespace geometry