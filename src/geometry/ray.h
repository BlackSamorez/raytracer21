#pragma once

#include <parameters.h>
#include <vector.h>

namespace geometry {
template <typename VectorNumericType = DefaultNumericType>
requires NumericTypeConstraint<VectorNumericType>
class Ray {
public:
    Ray(Vector3D<VectorNumericType> origin, Vector3D<VectorNumericType> direction)
        : origin_(origin), direction_(direction) {
    }

public:
    const Vector3D<VectorNumericType>& GetOrigin() const {
        return origin_;
    }

    const Vector3D<VectorNumericType>& GetDirection() const {
        return direction_;
    }

    void Propell(double magnitude) {
        origin_ += direction_ * magnitude;
    }

private:
    Vector3D<VectorNumericType> origin_;
    Vector3D<VectorNumericType> direction_;
};
}  // namespace geometry