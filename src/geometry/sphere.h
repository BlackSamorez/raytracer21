#pragma once

#include <parameters.h>
#include <vector.h>

namespace geometry {
template <typename VectorNumericType = DefaultNumericType>
class Sphere {
public:
    Sphere(Vector3D<VectorNumericType> center, double radius) : center_(center), radius_(radius) {
    }

public:
    const Vector3D<VectorNumericType>& GetCenter() const {
        return center_;
    }

    double GetRadius() const {
        return radius_;
    }

    bool ContainsOrigin(const Vector3D<VectorNumericType>& vector) const {
        return Length(vector - center_) < radius_;
    }

private:
    Vector3D<VectorNumericType> center_;
    double radius_;
};
}  // namespace geometry