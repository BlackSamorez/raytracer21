#pragma once

#include <vector.h>

class Sphere {
public:
    Sphere(Vector center, double radius) : center_(center), radius_(radius) {
    }

public:
    const Vector& GetCenter() const {
        return center_;
    }

    double GetRadius() const {
        return radius_;
    }

    bool ContainsOrigin(const Vector& vector) const {
        return Length(vector - center_) < radius_;
    }

private:
    Vector center_;
    double radius_;
};
