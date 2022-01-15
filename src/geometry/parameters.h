#pragma once

#include <concepts>

namespace geometry {
using DefaultNumericType = double;

template <typename NumericType>
concept NumericTypeConstraint = std::floating_point<NumericType>;
}  // namespace geometry
