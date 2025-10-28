#pragma once

#include <limits>
#include <numbers>
#include <numeric>
#include <random>

#include "glm/glm.hpp"

namespace mp {
constexpr auto infinity_f = std::numeric_limits<float>::max();
constexpr auto pi_f = std::numbers::pi_v<float>;

inline float random_float() {
  static std::random_device dev;
  static std::mt19937 gen(dev());
  static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
  return distribution(gen);
}

inline float random_float(const float min, const float max) {
  return min + (max - min) * random_float();
}

inline glm::vec3 random_vec() {
  return glm::vec3{random_float(), random_float(), random_float()};
}
inline glm::vec3 random_vec(const float min, const float max) {
  return glm::vec3{random_float(min, max), random_float(min, max),
                   random_float(min, max)};
}

inline glm::vec3 random_unit_vector() {
  while (true) {
    const auto p = random_vec(-1.0f, 1.0f);
    const auto pLengthSquared = dot(p, p);
    constexpr float kMinAcceptableValue = 1e-40f;
    if (kMinAcceptableValue < pLengthSquared && pLengthSquared <= 1) {
      return p / std::sqrt(pLengthSquared);
    }
  }
}

inline glm::vec3 random_on_hemisphere(const glm::vec3& normal) {
  const auto vectorOnHemisphere = random_unit_vector();
  if (dot(vectorOnHemisphere, normal) > 0.0f) {
    return vectorOnHemisphere;
  }
  return -vectorOnHemisphere;
}

inline bool near_zero(const float x) noexcept {
  constexpr float kEpsilon = 1e-6f;
  return std::fabs(x) < kEpsilon;
}

inline bool near_zero(const glm::vec3& x) noexcept {
  return near_zero(x.x) && near_zero(x.y) && near_zero(x.z);
}
inline constexpr glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) {
  return v - 2 * dot(v, n) * n;
}

inline glm::vec3 refract(const glm::vec3& direction, const glm::vec3 normal,
                         const float refractionRate) {
  const glm::vec3 refractDirectionOrth =
      refractionRate * (direction - dot(direction, normal) * normal);
  auto refractDirParLengthSquared =
      1 - dot(refractDirectionOrth, refractDirectionOrth);
  // Clamp to avoid negative due to floating-point errors
  refractDirParLengthSquared = std::max(0.0f, refractDirParLengthSquared);
  const glm::vec3 refractDirectionPar =
      std::sqrt(refractDirParLengthSquared) * -normal;
  return refractDirectionOrth + refractDirectionPar;
}

inline glm::vec3 random_in_unit_disk() {
  while (true) {
    const auto p =
        glm::vec3(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f), 0.0f);
    if (dot(p, p) < 1) {
      return p;
    }
  }
}

}  // namespace mp