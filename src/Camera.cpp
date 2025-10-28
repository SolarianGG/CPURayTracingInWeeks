#include <thread>
#include <vector>

#include "Camera.hpp"
#include "Material.hpp"
#include "glm/glm.hpp"

namespace mp {
bool Hit(const std::vector<Hittable>& objects, const Ray& ray,
         Interval<float> interval, HitRecord& hitRecord) {
  HitRecord tmpHitRecord;
  bool hitAnything = false;
  float currentMax = interval.max;

  for (const auto& o : objects) {
    if (o.hit(ray, Interval{.min = interval.min, .max = currentMax},
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
               const float vfov, const float defocusAngle,
               const float focusDistance, const glm::vec3& lookFrom,
               const glm::vec3& lookAt, const glm::vec3& worldUp)
    : m_image(baseWindowWidth, aspectRatio),
      m_cameraPos(lookFrom),
      m_samplesPerPixel(samplesPerPixel),
      m_pixelSamplesScale(1.0f / samplesPerPixel),
      m_maxDepth(maxDepth),
      m_defocusAngle(defocusAngle),
      m_focusDist(focusDistance) {
  const auto theta = glm::radians(vfov);
  const auto h = glm::tan(theta / 2);
  const float viewportHeight = 2.0f * h * focusDistance;
  const float viewportWidth =
      viewportHeight *
      (static_cast<double>(m_image.get_width()) / m_image.get_height());

  const auto cameraBack = normalize(lookFrom - lookAt);  // +Z
  const auto cameraRight =
      normalize(cross(worldUp, cameraBack));        // +X
  const auto cameraUp = cross(cameraBack, cameraRight);  // +Y

  const auto viewportU = cameraRight * viewportWidth;
  const auto viewportV = -cameraUp * viewportHeight;

  m_pixelDeltaU = viewportU / static_cast<float>(m_image.get_width());
  m_pixelDeltaV = viewportV / static_cast<float>(m_image.get_height());

  const auto viewportUpperLeft = m_cameraPos - focusDistance * cameraBack -
                                 viewportU / 2.0f - viewportV / 2.0f;
  m_startPixel = viewportUpperLeft + 0.5f * (m_pixelDeltaU + m_pixelDeltaV);

  const auto defocusRadius =
      focusDistance * std::tan(glm::radians(defocusAngle / 2));
  m_defocusDist_u = cameraRight * defocusRadius;
  m_defocusDist_v = cameraUp * defocusRadius;
}

const ImageBuffer& Camera::render(const std::vector<Hittable>& world) {
  const int width = m_image.get_width();
  const int height = m_image.get_height();
  const unsigned int threadCount = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;
  const int rowsPerThread = height / threadCount;
  int y = 0;
  auto renderRows = [this, &world, width](const int yStart, const int yEnd) {
    for (int y = yStart; y < yEnd; ++y) {
      for (int x = 0; x < width; ++x) {
        glm::vec3 finalColor{0, 0, 0};
        for (decltype(m_samplesPerPixel) i = 0; i < m_samplesPerPixel; ++i) {
          finalColor += ray_color(get_ray(x, y), m_maxDepth, world);
        }
        m_image[x, y] = Color::gamma(finalColor * m_pixelSamplesScale);
      }
    }
  };
  for (unsigned int t = 0; t < threadCount; ++t) {
    int yEnd = (t == threadCount - 1) ? height : y + rowsPerThread;
    threads.emplace_back(renderRows, y, yEnd);
    y = yEnd;
  }
  for (auto& th : threads) th.join();
  return m_image;
}

Ray Camera::get_ray(const int x, const int y) const {
  const auto offset = sample_square();

  const auto pixelSample =
      m_startPixel + ((static_cast<float>(x) + offset.x) * m_pixelDeltaU) +
      ((static_cast<float>(y) + offset.y) * m_pixelDeltaV);

  const auto rayOrigin =
      m_defocusAngle <= 0 ? m_cameraPos : defocus_disk_sample();
  return {rayOrigin, normalize(pixelSample - m_cameraPos)};
}

glm::vec3 Camera::defocus_disk_sample() const {
  const auto p = random_in_unit_disk();
  return m_cameraPos + (p.x * m_defocusDist_u) + (p.y * m_defocusDist_v);
}

inline glm::vec3 Camera::ray_color(const Ray& ray, const int depth,
                                   const std::vector<Hittable>& world) {
  if (depth <= 0) {
    return glm::vec3{};
  }
  HitRecord hitRecord;
  if (Hit(world, ray, mp::Interval<float>{.min = 0.001f, .max = infinity_f},
          hitRecord)) {
    Ray scattered;
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