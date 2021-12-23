#pragma once

#include <vector.h>

class Ray {
public:
    Ray(Vector origin, Vector direction) : origin_(origin), direction_(direction) {
    }

public:
    const Vector& GetOrigin() const {
        return origin_;
    }

    const Vector& GetDirection() const {
        return direction_;
    }

    void Propell(double magnitude) {
        origin_ += direction_ * magnitude;
    }

private:
    Vector origin_;
    Vector direction_;
};
