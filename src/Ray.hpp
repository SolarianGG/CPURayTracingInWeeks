#pragma once

#include "glm/glm.hpp"

namespace mp {

class Ray {
 public:
  constexpr Ray() = default;
  constexpr Ray(const glm::vec3& pos, const glm::vec3& direction)
      : m_pos(pos), m_direction(direction) {}

  [[nodiscard]]
  constexpr const glm::vec3& origin() const noexcept {
    return m_pos;
  }

  [[nodiscard]]
  constexpr const glm::vec3& direction() const noexcept {
    return m_direction;
  }

  [[nodiscard]]
  constexpr glm::vec3 at(const float t) const noexcept {
    return m_pos + m_direction * t;
  }

 private:
  glm::vec3 m_pos;
  glm::vec3 m_direction;
};
}  // namespace mp