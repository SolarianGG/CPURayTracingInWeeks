#pragma once

#include <vector>

#include "Hittable.hpp"
#include "ImageBuffer.hpp"
#include "Ray.hpp"
#include "Utility.hpp"

namespace mp {

class Camera final {
 public:
  explicit Camera(const std::size_t baseWindowWidth, const double aspectRatio,
                  const std::uint16_t samplesPerPixel, const int maxDepth,
                  const float vfov = 90.0f, const float defocusAngle = 0.0f,
                  const float focusDistance = 10.0f,
                  const glm::vec3& lookFrom = {0, 0, .0f},
                  const glm::vec3& lookAt = {0.0f, 0.0f, -1.0f},
                  const glm::vec3& worldUp = {0.0f, 1.0f, 0.0f});
  [[nodiscard]]
  const ImageBuffer& render(const std::vector<Hittable>& world);

 private:
  ImageBuffer m_image;
  glm::vec3 m_cameraPos;
  glm::vec3 m_startPixel{};
  glm::vec3 m_pixelDeltaU{};
  glm::vec3 m_pixelDeltaV{};
  std::uint16_t m_samplesPerPixel{};
  float m_pixelSamplesScale{};
  int m_maxDepth{};
  float m_defocusAngle;
  float m_focusDist;

  glm::vec3 m_defocusDist_u;
  glm::vec3 m_defocusDist_v;

  [[nodiscard]]
  Ray get_ray(const int x, const int y) const;

  [[nodiscard]]
  glm::vec3 defocus_disk_sample() const;

  [[nodiscard]]
  static glm::vec3 sample_square() {
    return glm::vec3{random_float() - 0.5f, random_float() - 0.5f, 0.0f};
  }

  [[nodiscard]]
  static glm::vec3 ray_color(const Ray& ray, const int depth,
                             const std::vector<Hittable>& world);
};
}  // namespace mp