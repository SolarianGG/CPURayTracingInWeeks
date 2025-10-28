#include "Material.hpp"

#include <memory>

#include "Utility.hpp"

namespace mp {
bool Lambertian::scatter(const Ray& rIn, const HitRecord& rec,
                         glm::vec3& attenuation, Ray& scattered) const {
  auto direction = rec.normal + random_unit_vector();
  if (near_zero(direction)) {
    direction = rec.normal;
  }
  scattered = Ray(rec.p, direction);
  attenuation = m_albedo;
  return true;
}

std::unique_ptr<Material> Lambertian::clone() const {
  return std::make_unique<Lambertian>(m_albedo);
}

bool Metal::scatter(const Ray& rIn, const HitRecord& rec,
                    glm::vec3& attenuation, Ray& scattered) const {
  const auto reflectedVec =
      normalize(mp::reflect(rIn.direction(), rec.normal));
  const auto fuzzedVec =
      reflectedVec + (random_unit_vector() * m_fuzzFactor);
  scattered = Ray(rec.p, fuzzedVec);
  attenuation = m_albedo;
  return dot(fuzzedVec, rec.normal) > 0;
}

std::unique_ptr<Material> Metal::clone() const {
  return std::make_unique<Metal>(m_albedo, m_fuzzFactor);
}

bool Dielectric::scatter(const Ray& rIn, const HitRecord& rec,
                         glm::vec3& attenuation, Ray& scattered) const {
  attenuation = glm::vec3(1.0f);
  const float ri = rec.frontFace ? (1.0f / m_refractionRate) : m_refractionRate;

  const auto unitRayDirection = normalize(rIn.direction());

  glm::vec3 direction;

  const float cosTheta =
      std::fmin(dot(-unitRayDirection, rec.normal), 1.0f);
  const float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

  if (const bool bCanRefract = ri * sinTheta <= 1.0f;
      !bCanRefract || reflectance(cosTheta, ri) > random_float()) {
    direction = mp::reflect(unitRayDirection, rec.normal);
  } else {
    direction = mp::refract(unitRayDirection, rec.normal, ri);
  }
  scattered = Ray(rec.p, direction);
  return true;
}

std::unique_ptr<Material> Dielectric::clone() const {
  return std::make_unique<Dielectric>(m_refractionRate);
}

float Dielectric::reflectance(const float cosTheta,
                              const float refractionRate) {
  const float r0 =
      std::pow((1.0f - refractionRate) / (1.0f + refractionRate), 2);
  return r0 + (1.0f - r0) * std::pow((1.0f - cosTheta), 5);
}
}  // namespace mp