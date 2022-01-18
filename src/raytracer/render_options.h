#pragma once

namespace raytracer {
enum class RenderMode { kDepth, kNormal, kFull };
struct RenderOptions {
    int depth = 4;
    RenderMode mode = RenderMode::kFull;
};
}  // namespace raytracer
