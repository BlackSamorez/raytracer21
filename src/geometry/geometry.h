#pragma once

#include <parameters.h>
#include <vector.h>
#include <sphere.h>
#include <intersection.h>
#include <triangle.h>
#include <ray.h>

#include <optional>

namespace geometry {
template <typename VectorNumericType>
requires std::floating_point<VectorNumericType>
std::optional<Intersection<VectorNumericType>> GetIntersection(
    const Ray<VectorNumericType>& ray, const Sphere<VectorNumericType>& sphere) {
    auto center = sphere.GetCenter() - ray.GetOrigin();

    auto center_ray_projection = DotProduct(center, ray.GetDirection());
    center_ray_projection /= Length(ray.GetDirection());

    auto normalized_ray_direction = ray.GetDirection();
    auto tc = normalized_ray_direction;
    tc.Normalize();
    tc *= center_ray_projection;

    auto d = tc - center;

    auto d_length = Length(d);

    if (d_length > sphere.GetRadius()) {
        return {};
    }

    auto delta_t = std::sqrt(sphere.GetRadius() * sphere.GetRadius() - d_length * d_length);

    Vector3D<VectorNumericType> relative_position;
    if (center_ray_projection > 0) {
        if (center_ray_projection - delta_t > 0) {
            relative_position = tc - normalized_ray_direction * delta_t;
        } else {
            relative_position = tc + normalized_ray_direction * delta_t;
        }
    } else {
        if (center_ray_projection + delta_t > 0) {
            relative_position = tc + normalized_ray_direction * delta_t;
        } else {
            return {};
        }
    }

    Vector3D<VectorNumericType> normal;
    if (Length(center) > sphere.GetRadius()) {
        normal = relative_position - center;
    } else {
        normal = center - relative_position;
    }

    normal.Normalize();

    return Intersection(relative_position + ray.GetOrigin(), normal, Length(relative_position));
}

template <typename VectorNumericType>
requires std::floating_point<VectorNumericType>
std::optional<Intersection<VectorNumericType>> GetIntersection(
    const Ray<VectorNumericType>& ray, const Triangle<VectorNumericType>& triangle) {
    const VectorNumericType calculation_epsilon = 0.0000001;
    auto vertex0 = triangle.GetVertex(0);
    auto vertex1 = triangle.GetVertex(1);
    auto vertex2 = triangle.GetVertex(2);
    Vector3D<VectorNumericType> edge1, edge2, h, s, q;
    VectorNumericType a, f, u, v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = CrossProduct(ray.GetDirection(), edge2);
    a = DotProduct(edge1, h);
    if (a > -calculation_epsilon && a < calculation_epsilon) {
        return {};  // This ray is parallel to this triangle.
    }
    f = 1.0 / a;
    s = ray.GetOrigin() - vertex0;
    u = f * DotProduct(s, h);
    if (u < 0.0 || u > 1.0) {
        return {};
    }
    q = CrossProduct(s, edge1);
    v = f * DotProduct(ray.GetDirection(), q);
    if (v < 0.0 || u + v > 1.0) {
        return {};
    }
    // At this stage we can compute t to find out where the intersection point is on the line.
    VectorNumericType t = f * DotProduct(edge2, q);
    if (t > calculation_epsilon)  // ray intersection
    {
        auto intersection_point = ray.GetOrigin() + ray.GetDirection() * t;
        auto distance = Length(intersection_point - ray.GetOrigin());

        auto normal = CrossProduct(edge1, edge2);
        normal.Normalize();
        if (DotProduct(normal, ray.GetDirection()) > 0) {
            normal -= 2 * normal;
        }

        return Intersection(intersection_point, normal, distance);
    } else {
        return {};  // This means that there is a line intersection but not a ray intersection.
    }
}

template <typename VectorNumericType>
requires std::floating_point<VectorNumericType>
Vector3D<VectorNumericType> GetBarycentricCoords(const Triangle<VectorNumericType>& triangle,
                                                 const Vector3D<VectorNumericType>& point) {
    auto s_bcx = Triangle({triangle.GetVertex(1), triangle.GetVertex(2), point}).Area() / 2;
    auto s_cax = Triangle({triangle.GetVertex(2), triangle.GetVertex(0), point}).Area() / 2;
    auto s_abx = Triangle({triangle.GetVertex(0), triangle.GetVertex(1), point}).Area() / 2;

    return Vector3D({s_bcx, s_cax, s_abx});
}

template <typename VectorNumericType, typename EtaNumericType>
requires std::floating_point<VectorNumericType> && std::floating_point<EtaNumericType>
std::optional<Vector3D<VectorNumericType>> Refract(const Vector3D<VectorNumericType>& ray,
                                                   const Vector3D<VectorNumericType>& normal,
                                                   EtaNumericType eta) {
    auto normalized_ray = ray;
    normalized_ray.Normalize();

    auto c = -DotProduct(normal, normalized_ray);
    if (eta * eta * (1 - c * c) > 1) {
        return {};
    }
    return eta * normalized_ray + (eta * c - std::sqrt(1 - eta * eta * (1 - c * c))) * normal;
}

template <typename VectorNumericType>
requires std::floating_point<VectorNumericType>
Vector3D<VectorNumericType> Reflect(Vector3D<VectorNumericType> ray,
                                    const Vector3D<VectorNumericType>& normal) {
    return ray - normal * DotProduct(normal, ray) * 2;
}
}  // namespace geometry