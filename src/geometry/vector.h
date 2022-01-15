#pragma once

#include <parameters.h>

#include <array>
#include <cmath>
#include <iostream>
#include <initializer_list>
#include <algorithm>

namespace geometry {

template <typename VectorNumericType = DefaultNumericType>
requires NumericTypeConstraint<VectorNumericType>
struct Vector3D {
public:
    Vector3D() : data_({0, 0, 0}) {
    }

    Vector3D(std::initializer_list<DefaultNumericType> initializer_list) {
        std::copy(initializer_list.begin(), initializer_list.end(), data_.begin());
    }

    explicit Vector3D(std::array<DefaultNumericType, 3> data) {
        std::copy(data.begin(), data.end(), data_.begin());
    }

public:
    VectorNumericType& operator[](size_t ind) {
        return data_[ind];
    }

    VectorNumericType operator[](size_t ind) const {
        return data_[ind];
    }

public:
    Vector3D<VectorNumericType> Normalize() {
        auto norm = Length(*this);
        for (int i = 0; i < 3; ++i) {
            data_[i] /= norm;
        }
        return *this;
    }

    [[nodiscard]] bool Zero() const {
        return Length(*this) == 0;
    }

    void operator+=(Vector3D<VectorNumericType> rhs) {
        data_[0] += rhs[0];
        data_[1] += rhs[1];
        data_[2] += rhs[2];
    }

    void operator-=(Vector3D<VectorNumericType> rhs) {
        data_[0] -= rhs[0];
        data_[1] -= rhs[1];
        data_[2] -= rhs[2];
    }

    void operator/=(VectorNumericType value) {
        data_[0] /= value;
        data_[1] /= value;
        data_[2] /= value;
    }

    void operator*=(VectorNumericType value) {
        data_[0] *= value;
        data_[1] *= value;
        data_[2] *= value;
    }

    Vector3D<VectorNumericType> operator-() {
        return Vector3D{-data_[0], -data_[1], -data_[2]};
    }

public:
    std::array<VectorNumericType, 3> data_{};
};

template <typename VectorNumericType>
requires NumericTypeConstraint<VectorNumericType>
inline VectorNumericType DotProduct(const Vector3D<VectorNumericType>& lhs,
                                    const Vector3D<VectorNumericType>& rhs) {
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}

template <typename VectorNumericType>
requires NumericTypeConstraint<VectorNumericType>
inline Vector3D<VectorNumericType> CrossProduct(const Vector3D<VectorNumericType>& a,
                                                const Vector3D<VectorNumericType>& b) {
    return Vector3D{a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
                    a[0] * b[1] - a[1] * b[0]};
}

template <typename VectorNumericType>
requires NumericTypeConstraint<VectorNumericType>
inline VectorNumericType Length(const Vector3D<VectorNumericType>& vec) {
    return std::sqrt(DotProduct(vec, vec));
}

template <typename VectorNumericType>
requires NumericTypeConstraint<VectorNumericType> Vector3D<VectorNumericType>
operator+(const Vector3D<VectorNumericType>& lhs, const Vector3D<VectorNumericType>& rhs) {
    Vector3D result = lhs;
    result += rhs;
    return result;
}

template <typename VectorNumericType>
requires NumericTypeConstraint<VectorNumericType> Vector3D<VectorNumericType>
operator-(const Vector3D<VectorNumericType>& lhs, const Vector3D<VectorNumericType>& rhs) {
    Vector3D result = lhs;
    result -= rhs;
    return result;
}

template <typename VectorNumericType>
requires NumericTypeConstraint<VectorNumericType> Vector3D<VectorNumericType>
operator*(const Vector3D<VectorNumericType>& vector, DefaultNumericType number) {
    Vector3D result = vector;
    result *= number;
    return result;
}

template <typename VectorNumericType>
Vector3D<VectorNumericType> operator/(const Vector3D<VectorNumericType>& vector,
                                      DefaultNumericType number) requires
    NumericTypeConstraint<VectorNumericType> {
    Vector3D result = vector;
    result /= number;
    return result;
}

template <typename VectorNumericType>
requires NumericTypeConstraint<VectorNumericType> Vector3D<VectorNumericType>
operator*(DefaultNumericType number, const Vector3D<VectorNumericType>& vector) {
    Vector3D result = vector;
    result *= number;
    return result;
}

template <typename VectorNumericType>
requires NumericTypeConstraint<VectorNumericType> Vector3D<VectorNumericType> LinearCombination(
    const Vector3D<VectorNumericType>& coefficients,
    const std::vector<Vector3D<VectorNumericType>*>& vectors) {
    Vector3D result = {0, 0, 0};
    for (size_t i = 0; i < vectors.size(); ++i) {
        result += coefficients[i] * *vectors[i];
    }
    return result;
}

template <typename VectorNumericType>
requires NumericTypeConstraint<VectorNumericType> Vector3D<VectorNumericType> LinearCombination(
    const Vector3D<VectorNumericType>& coefficients, const std::array<Vector3D<>*, 3>& vectors) {
    Vector3D result = {0, 0, 0};
    for (size_t i = 0; i < 3; ++i) {
        result += coefficients[i] * *vectors[i];
    }
    return result;
}

template <typename VectorNumericType>
requires NumericTypeConstraint<VectorNumericType> Vector3D<VectorNumericType>
operator*(const Vector3D<VectorNumericType>& lhs, const Vector3D<VectorNumericType>& rhs) {
    return Vector3D{lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2]};
}
}  // namespace geometry
