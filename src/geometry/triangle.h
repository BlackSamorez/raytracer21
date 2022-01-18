#pragma once

#include <geometry/parameters.h>
#include <geometry/vector.h>

namespace geometry {

template <typename VectorNumericType = DefaultNumericType>
requires NumericTypeConstraint<VectorNumericType>
class Triangle {
public:
    Triangle(std::initializer_list<Vector3D<VectorNumericType>> list) {
        std::copy(list.begin(), list.end(), vertices_.begin());
    }

public:
    VectorNumericType Area() const {
        return Length(CrossProduct(vertices_[1] - vertices_[0], vertices_[2] - vertices_[0])) / 2;
    }

    const Vector3D<VectorNumericType>& GetVertex(size_t ind) const {
        return vertices_[ind];
    }

private:
    std::array<Vector3D<VectorNumericType>, 3> vertices_;
};
}  // namespace geometry