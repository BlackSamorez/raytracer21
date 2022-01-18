#pragma once

#include <string>

#include "vector.h"
#include "material.h"
#include "scene.h"
#include "light.h"
#include "object.h"
#include "ray.h"
#include "geometry.h"
#include "skybox.h"

namespace raytracer {
const double kEpsilon = 0.0001;

std::pair<std::optional<geometry::Intersection<>>, const scene::Material*>
GetIntersectionAndMaterial(const geometry::Ray<>& ray, const scene::Object& object) {
    auto trivial_normal_intersection = GetIntersection(ray, object.polygon);

    if (!trivial_normal_intersection) {
        return {trivial_normal_intersection, object.material};
    }

    for (size_t i = 0; i < object.normals.size(); ++i) {
        if (object.normals[i] == nullptr) {
            return {trivial_normal_intersection, object.material};
        }
    }
    auto true_normal = LinearCombination(
        GetBarycentricCoords(object.polygon, trivial_normal_intersection.value().GetPosition()),
        object.normals);

    return {geometry::Intersection{trivial_normal_intersection->GetPosition(), true_normal,
                                   trivial_normal_intersection->GetDistance()},
            object.material};
}

inline std::pair<std::optional<geometry::Intersection<>>, const scene::Material*>
GetIntersectionAndMaterial(const geometry::Ray<>& ray, const scene::SphereObject& sphere_object) {
    return {GetIntersection(ray, sphere_object.sphere), sphere_object.material};
}

std::vector<std::pair<geometry::Intersection<>, const scene::Material*>>
FindAllIntersectionsAndMaterials(const scene::Scene& scene, const geometry::Ray<>& ray) {
    std::vector<std::pair<geometry::Intersection<>, const scene::Material*>> intersections;
    for (const auto& object : scene.GetObjects()) {
        auto [possible_intersection, material] = GetIntersectionAndMaterial(ray, object);
        if (possible_intersection) {
            intersections.emplace_back(possible_intersection.value(), material);
        }
    }

    for (const auto& sphere_object : scene.GetSphereObjects()) {
        auto [possible_intersection, material] = GetIntersectionAndMaterial(ray, sphere_object);
        if (possible_intersection) {
            intersections.emplace_back(possible_intersection.value(), material);
        }
    }
    return intersections;
}

std::pair<std::optional<geometry::Intersection<>>, const scene::Material*>
FindClosestIntersectionAndMaterial(
    std::vector<std::pair<geometry::Intersection<>, const scene::Material*>> intersections) {
    if (!intersections.empty()) {
        return *std::min_element(
            intersections.begin(), intersections.end(),
            [](auto& lhs, auto& rhs) { return lhs.first.GetDistance() < rhs.first.GetDistance(); });

    } else {
        return {{}, nullptr};
    }
}

std::pair<std::optional<geometry::Intersection<>>, const scene::Material*>
FindClosestIntersectionAndMaterial(const scene::Scene& scene, const geometry::Ray<>& ray) {
    return FindClosestIntersectionAndMaterial(FindAllIntersectionsAndMaterials(scene, ray));
}

inline bool ReachThroughPossible(const scene::Material* material) {
    return material->refraction_index == 1 && material->albedo[2] != 0;
}

geometry::Vector3D<> LightReach(const scene::Scene& scene, const scene::Light& light,
                                const geometry::Vector3D<>& position, int ttl) {
    if (ttl < 0) {
        return {0, 0, 0};
    }
    geometry::Vector3D<> ray_direction = position - light.position;
    ray_direction.Normalize();
    geometry::Ray ray(light.position, ray_direction);
    auto [closest_intersection, material] = FindClosestIntersectionAndMaterial(scene, ray);

    if (!closest_intersection) {
        return {0, 0, 0};
    }

    if (Length(closest_intersection.value().GetPosition() - position) < kEpsilon) {
        return light.intensity;
    }

    if (ReachThroughPossible(material)) {
        scene::Light new_light = {closest_intersection.value().GetPosition(),
                                  material->albedo[2] * material->specular_color * light.intensity};
        new_light.position += ray_direction * kEpsilon;
        return LightReach(scene, new_light, position, ttl - 1);
    } else {
        return {0, 0, 0};
    }
}

geometry::Vector3D<> CalculateDiffuse(const scene::Scene& scene,
                                      const geometry::Intersection<>& intersection, int ttl) {
    geometry::Vector3D<> total_diffusive_illumination = {0, 0, 0};
    for (const auto& light : scene.GetLights()) {
        auto illumination = LightReach(scene, light, intersection.GetPosition(), ttl);
        if (!illumination.Zero()) {
            auto light_direction = (light.position - intersection.GetPosition()).Normalize();
            total_diffusive_illumination +=
                illumination * std::max(0.0, DotProduct(light_direction, intersection.GetNormal()));
        }
    }
    return total_diffusive_illumination;
}

geometry::Vector3D<> CalculateSpecular(const scene::Scene& scene,
                                       const geometry::Intersection<>& intersection,
                                       const scene::Material* material, const geometry::Ray<>& ray,
                                       int ttl) {
    geometry::Vector3D<> total_specular_illumination{0, 0, 0};
    for (const auto& light : scene.GetLights()) {
        auto illumination = LightReach(scene, light, intersection.GetPosition(), ttl);
        if (!illumination.Zero()) {
            auto light_direction = (light.position - intersection.GetPosition()).Normalize();
            auto reflection_direction =
                Reflect(-light_direction, intersection.GetNormal()).Normalize();
            double cos_sigma = -DotProduct(reflection_direction, ray.GetDirection());

            total_specular_illumination +=
                illumination * pow(std::max(0.0, cos_sigma), material->specular_exponent);
        }
    }
    return total_specular_illumination;
}

geometry::Vector3D<> CalculateIllumination(const scene::Scene& scene, const geometry::Ray<>& ray,
                                           bool inside, int ttl) {
    if (ttl < 0) {
        return geometry::Vector3D<>{0, 0, 0};
    }

    auto [possible_intersection, material] = FindClosestIntersectionAndMaterial(scene, ray);
    if (!possible_intersection) {
        return scene.sky_.Trace(ray);
    }
    auto intersection = possible_intersection.value();

    // Ambient
    auto illumination_ambient = material->ambient_color + material->intensity;

    // Diffusive
    auto illumination_diffusive = material->diffuse_color *
                                  CalculateDiffuse(scene, intersection, ttl - 1) *
                                  material->albedo[0];

    // Specular
    auto illumination_specular = material->specular_color *
                                 CalculateSpecular(scene, intersection, material, ray, ttl - 1) *
                                 material->albedo[0];

    // Reflected
    geometry::Ray reflected_ray = {intersection.GetPosition(),
                                   Reflect(ray.GetDirection(), intersection.GetNormal())};
    reflected_ray.Propell(kEpsilon);
    geometry::Vector3D<> illumination_reflected;
    if (material->albedo[1] != 0 && !inside) {
        illumination_reflected = material->specular_color *
                                 CalculateIllumination(scene, reflected_ray, false, ttl - 1) *
                                 material->albedo[1];
    } else {
        illumination_reflected = {0, 0, 0};
    }

    // Refracted
    geometry::Vector3D<> illumination_refracted;
    std::optional<geometry::Vector3D<>> refracted_ray_direction;
    if (!inside) {
        refracted_ray_direction =
            Refract(ray.GetDirection(), intersection.GetNormal(), 1 / material->refraction_index);
    } else {
        refracted_ray_direction =
            Refract(ray.GetDirection(), intersection.GetNormal(), material->refraction_index);
        //        refracted_ray_direction = {};
    }

    if (refracted_ray_direction && (material->albedo[2] != 0)) {
        geometry::Ray refracted_ray = {intersection.GetPosition(), refracted_ray_direction.value()};
        refracted_ray.Propell(kEpsilon);

        illumination_refracted = material->specular_color *
                                 CalculateIllumination(scene, refracted_ray, true, ttl - 1) *
                                 material->albedo[2];
    } else {
        illumination_refracted = {0, 0, 0};
    }

    if (inside && material->albedo[2] != 0) {
        illumination_refracted *= (material->albedo[2] + material->albedo[1]) / material->albedo[2];
    }

    return illumination_ambient + illumination_diffusive + illumination_specular +
           illumination_reflected + illumination_refracted;
}
}  // namespace raytracer
