#pragma once

#include <vector.h>

class Triangle {
public:
    Triangle(std::initializer_list<Vector> list) {
        std::copy(list.begin(), list.end(), vertices_.begin());
    }

public:
    double Area() const {
        return Length(CrossProduct(vertices_[1] - vertices_[0], vertices_[2] - vertices_[0])) / 2;
    }

    const Vector& GetVertex(size_t ind) const {
        return vertices_[ind];
    }

private:
    std::array<Vector, 3> vertices_;
};
