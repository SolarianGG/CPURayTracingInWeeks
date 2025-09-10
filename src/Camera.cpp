#include "Camera.hpp"

#include "Material.hpp"
#include "glm/glm.hpp"

namespace mp {
bool Hit(const std::vector<mp::Hittable>& objects, const mp::Ray& ray,
         mp::Interval<float> interval, mp::HitRecord& hitRecord) {
  mp::HitRecord tmpHitRecord;
  bool hitAnything = false;
  float currentMax = interval.max;

  for (const auto& o : objects) {
    if (o.hit(ray, mp::Interval{.min = interval.min, .max = currentMax},
              tmpHitRecord)) {
      hitRecord = tmpHitRecord;
      currentMax = tmpHitRecord.t;
      hitAnything = true;
    }
  }

  return hitAnything;
}

Camera::Camera(const std::size_t baseWindowWidth, const double aspectRatio,
               const std::uint16_t samplesPerPixel, const int maxDepth,
               const float vfov, const glm::vec3& lookFrom,
               const glm::vec3& lookAt, const glm::vec3& worldUp)
    : m_image(baseWindowWidth, aspectRatio),
      m_cameraPos(lookFrom),
      m_samplesPerPixel(samplesPerPixel),
      m_pixelSamplesScale(1.0f / samplesPerPixel),
      m_maxDepth(maxDepth) {
  const float focalLength = (lookAt - lookFrom).length();
  const auto theta = glm::radians(vfov);
  const auto h = glm::tan(theta / 2);
  const float viewportHeight = 2.0 * h * focalLength;
  const float viewportWidth =
      viewportHeight *
      (static_cast<double>(m_image.get_width()) / m_image.get_height());

  const auto cameraBack = glm::normalize(lookFrom - lookAt);  // +Z
  const auto cameraRight =
      glm::normalize(glm::cross(worldUp, cameraBack));        // +X
  const auto cameraUp = glm::cross(cameraBack, cameraRight);  // +Y

  const auto viewportU = cameraRight * viewportWidth;
  const auto viewportV = -cameraUp * viewportHeight;

  m_pixelDeltaU = viewportU / static_cast<float>(m_image.get_width());
  m_pixelDeltaV = viewportV / static_cast<float>(m_image.get_height());

  const auto viewportUpperLeft = m_cameraPos - focalLength * cameraBack -
                                 viewportU / 2.0f - viewportV / 2.0f;
  m_startPixel = viewportUpperLeft + 0.5f * (m_pixelDeltaU + m_pixelDeltaV);
}

const ImageBuffer& Camera::render(const std::vector<mp::Hittable>& world) {
  for (int y = 0; y < m_image.get_height(); ++y) {
    for (int x = 0; x < m_image.get_width(); ++x) {
      glm::vec3 finalColor{0, 0, 0};
      for (decltype(m_samplesPerPixel) i = 0; i < m_samplesPerPixel; ++i) {
        finalColor += ray_color(get_ray(x, y), m_maxDepth, world);
      }
      m_image[x, y] = mp::Color::gamma(finalColor * m_pixelSamplesScale);
    }
  }

  return m_image;
}

mp::Ray Camera::get_ray(const int x, const int y) const {
  const auto offset = sample_square();

  const auto pixelSample =
      m_startPixel + ((static_cast<float>(x) + offset.x) * m_pixelDeltaU) +
      ((static_cast<float>(y) + offset.y) * m_pixelDeltaV);

  return mp::Ray(m_cameraPos, glm::normalize(pixelSample - m_cameraPos));
}

inline glm::vec3 Camera::ray_color(const mp::Ray& ray, const int depth,
                                   const std::vector<mp::Hittable>& world) {
  if (depth <= 0) {
    return glm::vec3{};
  }
  mp::HitRecord hitRecord;
  if (Hit(world, ray, mp::Interval<float>{.min = 0.001f, .max = mp::infinity_f},
          hitRecord)) {
    mp::Ray scattered;
    glm::vec3 att;
    if (hitRecord.mat->scatter(ray, hitRecord, att, scattered)) {
      return att * ray_color(scattered, depth - 1, world);
    }
    return glm::vec3{};
  }
  const auto a = 0.5f * (ray.direction().y + 1.0f);
  return mix(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.7f, 1.0f), a);
}
}  // namespace mp