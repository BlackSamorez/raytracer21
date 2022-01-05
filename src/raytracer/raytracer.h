#pragma once

#include <image.h>
#include <camera_options.h>
#include <render_options.h>
#include <scene.h>
#include <geometry.h>
#include <skybox.h>

#include <string>

const double kEpsilon = 0.0001;

std::pair<std::optional<geometry::Intersection<>>, const Material*> GetIntersectionAndItsMaterial(
    const geometry::Ray<>& ray, const Object& object) {
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
//    true_normal.Normalize();

    return {geometry::Intersection{trivial_normal_intersection->GetPosition(), true_normal,
                                   trivial_normal_intersection->GetDistance()},
            object.material};
}

inline std::pair<std::optional<geometry::Intersection<>>, const Material*>
GetIntersectionAndItsMaterial(const geometry::Ray<>& ray, const SphereObject& sphere_object) {
    return {GetIntersection(ray, sphere_object.sphere), sphere_object.material};
}

class RayCaster {
public:
    explicit RayCaster(const CameraOptions& camera_options) {
        screen_height_ = camera_options.screen_height;
        screen_width_ = camera_options.screen_width;

        origin_ = camera_options.look_from;
        backward_unit_ = camera_options.look_from - camera_options.look_to;
        backward_unit_.Normalize();

        right_unit_ = CrossProduct({0, 1, 0}, backward_unit_);

        if (Length(right_unit_) < kEpsilon) {
            right_unit_ = {1, 0, 0};
            up_unit_ = {0, 0, 1};
        } else {
            right_unit_.Normalize();
            up_unit_ = CrossProduct(right_unit_, backward_unit_);
            up_unit_.Normalize();
        }

        double pixel_size = 2 * std::tan(camera_options.fov / 2) / screen_height_;
        up_unit_ *= pixel_size;
        right_unit_ *= pixel_size;

        if (backward_unit_[1] < 0) {
//            up_unit_ *= -1;  // I have no fucking idea why
        }
    }

public:
    geometry::Ray<> operator()(int horizontal_pixel_index, int vertical_pixel_index) const {
        auto direction =
            (static_cast<double>((2 * horizontal_pixel_index - screen_width_ + 1)) / 2) *
                right_unit_ +
            (static_cast<double>((2 * vertical_pixel_index - screen_height_ + 1)) / 2) * up_unit_ -
            backward_unit_;
        direction.Normalize();
        return {origin_, direction};
    }

private:
    geometry::Vector3D<> origin_;
    geometry::Vector3D<> backward_unit_;

    geometry::Vector3D<> right_unit_;
    geometry::Vector3D<> up_unit_;

    int screen_height_;
    int screen_width_;
};

std::pair<std::optional<geometry::Intersection<>>, const Material*>
FindClosestIntersectionAndItsMaterial(const Scene& scene, const geometry::Ray<>& ray) {
    std::vector<std::pair<geometry::Intersection<>, const Material*>> intersections;
    for (const auto& object : scene.GetObjects()) {
        auto possible_intersection = GetIntersectionAndItsMaterial(ray, object);
        if (possible_intersection.first) {
            intersections.emplace_back(possible_intersection.first.value(),
                                       possible_intersection.second);
        }
    }

    for (const auto& sphere_object : scene.GetSphereObjects()) {
        auto possible_intersection = GetIntersectionAndItsMaterial(ray, sphere_object);
        if (possible_intersection.first) {
            intersections.emplace_back(possible_intersection.first.value(),
                                       possible_intersection.second);
        }
    }

    if (!intersections.empty()) {
        // Select the closest one
        auto closest_pair_it =
            std::min_element(intersections.begin(), intersections.end(),
                             [](const std::pair<geometry::Intersection<>, const Material*>& lhs,
                                const std::pair<geometry::Intersection<>, const Material*>& rhs) {
                                 return lhs.first.GetDistance() < rhs.first.GetDistance();
                             });
        return *closest_pair_it;
    } else {
        std::optional<geometry::Intersection<>> result = {};
        return {result, nullptr};
    }
}

void ToneMappingAndGammaCorrection(geometry::Vector3D<>* pseudo_pixels, int number_of_pixels) {
    double coefficient = 0;
    for (int pixel_index = 0; pixel_index < number_of_pixels; ++pixel_index) {
        for (int color_index = 0; color_index < 3; ++color_index) {
            coefficient = std::max(coefficient, pseudo_pixels[pixel_index][color_index]);
        }
    }

    for (int pixel_index = 0; pixel_index < number_of_pixels; ++pixel_index) {
        for (int color_index = 0; color_index < 3; ++color_index) {
            pseudo_pixels[pixel_index][color_index] =
                pseudo_pixels[pixel_index][color_index] *
                (1 + pseudo_pixels[pixel_index][color_index] / (coefficient * coefficient)) /
                (1 + pseudo_pixels[pixel_index][color_index]);
            pseudo_pixels[pixel_index][color_index] =
                std::pow(pseudo_pixels[pixel_index][color_index], 1 / 2.2);
        }
    }
}

bool ReachThroughPossible(const Material* material) {
    return material->refraction_index == 1 && material->albedo[2] != 0;
}

geometry::Vector3D<> LightReach(const Scene& scene, const Light& light,
                                const geometry::Vector3D<>& position, int ttl) {
    if (ttl < 0) {
        return {0, 0, 0};
    }
    geometry::Vector3D<> ray_direction = position - light.position;
    ray_direction.Normalize();
    geometry::Ray ray(light.position, ray_direction);
    auto [closest_intersection, material] = FindClosestIntersectionAndItsMaterial(scene, ray);

    if (!closest_intersection) {
        return {0, 0, 0};
    }

    if (Length(closest_intersection.value().GetPosition() - position) < kEpsilon) {
        return light.intensity;
    }

    if (ReachThroughPossible(material)) {
        Light new_light = {closest_intersection.value().GetPosition(),
                           material->albedo[2] * material->specular_color * light.intensity};
        new_light.position += ray_direction * kEpsilon;
        return LightReach(scene, new_light, position, ttl - 1);
    } else {
        return {0, 0, 0};
    }
}

geometry::Vector3D<> CalculateDiffuseIllumination(const Scene& scene,
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

geometry::Vector3D<> CalculateSpecularIllumination(const Scene& scene,
                                                   const geometry::Intersection<>& intersection,
                                                   const Material* material,
                                                   const geometry::Ray<>& ray, int ttl) {
    geometry::Vector3D<> total_specular_illumination{0, 0, 0};
    for (const auto& light : scene.GetLights()) {
        auto illumination = LightReach(scene, light, intersection.GetPosition(), ttl);
        if (!illumination.Zero()) {
            auto light_direction = (light.position - intersection.GetPosition()).Normalize();
            auto reflection_direction =
                Reflect(-light_direction, intersection.GetNormal()).Normalize();
            double cos_sigma = -DotProduct(reflection_direction, ray.GetDirection());

            total_specular_illumination +=
                illumination * std::pow(std::max(0.0, cos_sigma), material->specular_exponent);
        }
    }
    return total_specular_illumination;
}

geometry::Vector3D<> CalculateIllumination(const Scene& scene, const geometry::Ray<>& ray,
                                           bool inside, int ttl) {
    if (ttl < 0) {
        return geometry::Vector3D<>{0, 0, 0};
    }

    auto [possible_intersection, material] = FindClosestIntersectionAndItsMaterial(scene, ray);
    if (!possible_intersection) {
        return scene.sky_.Trace(ray);
    }
    auto intersection = possible_intersection.value();

    // Ambient
    auto illumination_ambient = material->ambient_color + material->intensity;

    // Diffusive
    auto illumination_diffusive = material->diffuse_color *
                                  CalculateDiffuseIllumination(scene, intersection, ttl - 1) *
                                  material->albedo[0];

    // Specular
    auto illumination_specular = material->specular_color *
                                 CalculateSpecularIllumination(scene, intersection, material, ray, ttl - 1) *
                                 material->albedo[0];

    // Reflected
    geometry::Ray reflected_ray = {intersection.GetPosition(),
                                   Reflect(ray.GetDirection(), intersection.GetNormal())};
    reflected_ray.Propell(kEpsilon);  // TODO check if works
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

Image Render(const std::string& filename, const CameraOptions& camera_options,
             const RenderOptions& render_options) {
    Image image(camera_options.screen_width, camera_options.screen_height);
    image.PrepareImage(image.Width(), image.Height());
    auto scene = ReadScene(filename);
    RayCaster basic_screen_thrower(camera_options);

    if (render_options.mode == RenderMode::kDepth) {
        auto* depths = new double[image.Width() * image.Height()];

        double deepest_depth = 0;
        for (int horizontal_pixel_index = 0; horizontal_pixel_index < image.Width();
             ++horizontal_pixel_index) {
            for (int vertical_pixel_index = 0; vertical_pixel_index < image.Height();
                 ++vertical_pixel_index) {
                auto cast_ray = basic_screen_thrower(horizontal_pixel_index, vertical_pixel_index);
                auto possible_intersection =
                    FindClosestIntersectionAndItsMaterial(scene, cast_ray).first;

                if (possible_intersection) {
                    // Remember the greatest distance
                    deepest_depth =
                        std::max(deepest_depth, possible_intersection.value().GetDistance());

                    // Save pixel value
                    depths[image.Width() * vertical_pixel_index + horizontal_pixel_index] =
                        possible_intersection.value().GetDistance();
                } else {
                    depths[image.Width() * vertical_pixel_index + horizontal_pixel_index] = 0;
                }
            }
        }
        // Normalize
        for (int i = 0; i < image.Width() * image.Height(); ++i) {
            if (depths[i] == 0) {
                depths[i] = 1;
            } else {
                depths[i] /= deepest_depth;
            }
        }

        // Build pixels
        for (int horizontal_pixel_index = 0; horizontal_pixel_index < image.Width();
             ++horizontal_pixel_index) {
            for (int vertical_pixel_index = 0; vertical_pixel_index < image.Height();
                 ++vertical_pixel_index) {
                int brightness = static_cast<int>(
                    (depths[image.Width() * vertical_pixel_index + horizontal_pixel_index] -
                     kEpsilon) *
                    256);
                image.SetPixel({brightness, brightness, brightness}, vertical_pixel_index,
                               horizontal_pixel_index);
            }
        }
    }

    if (render_options.mode == RenderMode::kNormal) {
        auto* pseudo_pixels = new geometry::Vector3D<>[image.Width() * image.Height()];

        for (int horizontal_pixel_index = 0; horizontal_pixel_index < image.Width();
             ++horizontal_pixel_index) {
            for (int vertical_pixel_index = 0; vertical_pixel_index < image.Height();
                 ++vertical_pixel_index) {
                auto cast_ray = basic_screen_thrower(horizontal_pixel_index, vertical_pixel_index);
                // Check all possible intersections
                auto possible_intersection =
                    FindClosestIntersectionAndItsMaterial(scene, cast_ray).first;

                if (possible_intersection) {
                    // Save pixel value
                    pseudo_pixels[image.Width() * vertical_pixel_index + horizontal_pixel_index] =
                        possible_intersection.value().GetNormal();
                } else {
                    pseudo_pixels[image.Width() * vertical_pixel_index + horizontal_pixel_index] = {
                        -1, -1, -1};
                }
            }
        }
        // Normalize
        for (int i = 0; i < image.Width() * image.Height(); ++i) {
            pseudo_pixels[i] /= 2;
            pseudo_pixels[i] += geometry::Vector3D<>{0.5, 0.5, 0.5};
        }

        // Build pixels
        for (int horizontal_pixel_index = 0; horizontal_pixel_index < image.Width();
             ++horizontal_pixel_index) {
            for (int vertical_pixel_index = 0; vertical_pixel_index < image.Height();
                 ++vertical_pixel_index) {
                auto pseudo_pixel =
                    pseudo_pixels[image.Width() * vertical_pixel_index + horizontal_pixel_index];

                int red = static_cast<int>((pseudo_pixel[0] - kEpsilon) * 256);
                int green = static_cast<int>((pseudo_pixel[1] - kEpsilon) * 256);
                int blue = static_cast<int>((pseudo_pixel[2] - kEpsilon) * 256);
                image.SetPixel({red, green, blue}, vertical_pixel_index, horizontal_pixel_index);
            }
        }
    }

    if (render_options.mode == RenderMode::kFull) {
        auto* pseudo_pixels = new geometry::Vector3D<>[image.Width() * image.Height()];

        for (int horizontal_pixel_index = 0; horizontal_pixel_index < image.Width();
             ++horizontal_pixel_index) {
            for (int vertical_pixel_index = 0; vertical_pixel_index < image.Height();
                 ++vertical_pixel_index) {
                if (horizontal_pixel_index == 325 && vertical_pixel_index == 250) {
                    continue;
                }
                auto cast_ray = basic_screen_thrower(horizontal_pixel_index, vertical_pixel_index);
                pseudo_pixels[image.Width() * vertical_pixel_index + horizontal_pixel_index] =
                    CalculateIllumination(scene, cast_ray, false, render_options.depth);
            }
        }
        // Normalize
        ToneMappingAndGammaCorrection(pseudo_pixels, image.Width() * image.Height());
        // Build pixels
        for (int horizontal_pixel_index = 0; horizontal_pixel_index < image.Width();
             ++horizontal_pixel_index) {
            for (int vertical_pixel_index = 0; vertical_pixel_index < image.Height();
                 ++vertical_pixel_index) {
                auto pseudo_pixel =
                    pseudo_pixels[image.Width() * vertical_pixel_index + horizontal_pixel_index];

                int red = static_cast<int>((pseudo_pixel[0] - kEpsilon) * 256);
                int green = static_cast<int>((pseudo_pixel[1] - kEpsilon) * 256);
                int blue = static_cast<int>((pseudo_pixel[2] - kEpsilon) * 256);
                image.SetPixel({red, green, blue}, vertical_pixel_index, horizontal_pixel_index);
            }
        }
    }

    return image;
}
