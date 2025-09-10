#pragma once

#include "Interval.hpp"
#include "glm/glm.hpp"

namespace mp {

#pragma pack(push, 1)
struct Color {
  std::uint8_t r, g, b;

  static constexpr Interval<float> kIntensity{.min = 0.0f, .max = 0.999f};

  constexpr static Color black() noexcept { return Color{0, 0, 0}; }
  constexpr static Color white() noexcept { return Color{255, 255, 255}; }
  constexpr static Color red() noexcept { return Color(255, 0, 0); }
  constexpr static Color green() noexcept { return Color{0, 255, 0}; }
  constexpr static Color blue() noexcept { return Color{0, 0, 255}; }

  static Color gamma(const glm::vec3& color) {
    return Color{gamma(color.r), gamma(color.g), gamma(color.b)};
  }

  static float gamma(const float channel) {
    if (channel > 0) {
      return std::sqrt(channel);
    }
    return 0.0f;
  }

  constexpr explicit Color(const int ir = 0, const int ig = 0, const int ib = 0)
      : r(ir), g(ig), b(ib) {}
  constexpr explicit Color(const float dr, const float dg = 0.0f,
                           const float db = 0.0f)
      : r(static_cast<std::uint8_t>(kIntensity.clamp(dr) * 256)),
        g(static_cast<std::uint8_t>(kIntensity.clamp(dg) * 256)),
        b(static_cast<std::uint8_t>(kIntensity.clamp(db) * 256)) {}
  constexpr explicit Color(const glm::vec3& vec) : Color(vec.r, vec.g, vec.b) {}
};
#pragma pack(pop)
}  // namespace mp