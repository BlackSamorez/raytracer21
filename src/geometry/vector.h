#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <initializer_list>
#include <algorithm>

class Vector3D {
public:
    Vector3D() {
        Vector3D({0, 0, 0});
    }

    Vector3D(std::initializer_list<double> list) {
        std::copy(list.begin(), list.end(), data_.begin());
    }

    Vector3D(std::array<double, 3> data) {
        std::copy(data.begin(), data.end(), data_.begin());
    }

public:
    double& operator[](size_t ind) {
        return data_[ind];
    }

    double operator[](size_t ind) const {
        return data_[ind];
    }

public:
    void Normalize() {
        double norm = Length(*this);
        for (int i = 0; i < 3; ++i) {
            data_[i] /= norm;
        }
    }

    bool Zero() {
        return Length(*this) == 0;
    }

    friend inline double DotProduct(const Vector3D& lhs, const Vector3D& rhs);

    friend inline Vector3D CrossProduct(const Vector3D& a, const Vector3D& b);

    friend inline double Length(const Vector3D& vec);

    void operator+=(Vector3D rhs) {
        data_[0] += rhs[0];
        data_[1] += rhs[1];
        data_[2] += rhs[2];
    }

    void operator-=(Vector3D rhs) {
        data_[0] -= rhs[0];
        data_[1] -= rhs[1];
        data_[2] -= rhs[2];
    }

    void operator/=(double value) {
        data_[0] /= value;
        data_[1] /= value;
        data_[2] /= value;
    }

    void operator*=(double value) {
        data_[0] *= value;
        data_[1] *= value;
        data_[2] *= value;
    }

    friend Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs);

    friend Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs);

    friend Vector3D operator*(const Vector3D& vector, double number);

    friend Vector3D operator*(double number, const Vector3D& vector);

    friend Vector3D operator/(const Vector3D& vector, double number);

    friend Vector3D operator*(const Vector3D& lhs, const Vector3D& rhs);

private:
    std::array<double, 3> data_;
};

inline double DotProduct(const Vector3D& lhs, const Vector3D& rhs) {
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}

inline Vector3D CrossProduct(const Vector3D& a, const Vector3D& b) {
    return Vector3D(
        {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]});
}

inline double Length(const Vector3D& vec) {
    return std::sqrt(DotProduct(vec, vec));
}

Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs) {
    Vector3D result = lhs;
    result += rhs;
    return result;
}

Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs) {
    Vector3D result = lhs;
    result -= rhs;
    return result;
}

Vector3D operator*(const Vector3D& vector, double number) {
    Vector3D result = vector;
    result *= number;
    return result;
}

Vector3D operator/(const Vector3D& vector, double number) {
    Vector3D result = vector;
    result /= number;
    return result;
}

Vector3D operator*(double number, const Vector3D& vector) {
    Vector3D result = vector;
    result *= number;
    return result;
}

Vector3D LinearCombination(const Vector3D& coefficients, const std::vector<Vector*>& vectors) {
    Vector3D result = {0, 0, 0};
    for (size_t i = 0; i < vectors.size(); ++i) {
        result += coefficients[i] * *vectors[i];
    }
    return result;
}

Vector3D operator*(const Vector3D& lhs, const Vector3D& rhs) {
    return Vector3D{lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2]};
}
