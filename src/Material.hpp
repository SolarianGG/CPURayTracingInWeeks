#pragma once
#include "Hittable.hpp"
#include "Ray.hpp"

namespace mp {

class Material {
 public:
  Material(const Material& other) = delete;
  Material(Material&& other) noexcept = delete;
  Material& operator=(const Material& other) = delete;
  Material& operator=(Material&& other) noexcept = delete;
  Material() = default;

  virtual ~Material() = default;

  [[nodiscard]]
  virtual bool scatter(const Ray& rIn, const HitRecord& rec,
                       glm::vec3& attenuation, Ray& scattered) const = 0;

  [[nodiscard]]
  virtual std::unique_ptr<Material> clone() const = 0;
};

class Lambertian final : public Material {
 public:
  explicit Lambertian(const glm::vec3& a) : m_albedo(a) {}

  [[nodiscard]]
  bool scatter(const Ray& rIn, const HitRecord& rec, glm::vec3& attenuation,
               Ray& scattered) const override;

  [[nodiscard]] std::unique_ptr<Material> clone() const override;

 private:
  glm::vec3 m_albedo;
};

class Metal final : public Material {
 public:
  explicit Metal(const glm::vec3& a, const float fuzz)
      : m_albedo(a), m_fuzzFactor(fuzz) {}

  [[nodiscard]]
  bool scatter(const Ray& rIn, const HitRecord& rec, glm::vec3& attenuation,
               Ray& scattered) const override;

  [[nodiscard]] std::unique_ptr<Material> clone() const override;

 private:
  glm::vec3 m_albedo;
  float m_fuzzFactor;
};

class Dielectric final : public Material {
 public:
  static constexpr float kRefractionRateVacuum = 1.0f;
  static constexpr float kRefractionRateAir = 1.000273f;
  static constexpr float kRefractionRateGlass = 1.52f;
  static constexpr float kRefractionRateWater = 1.333f;
  static constexpr float kRefractionRateDiamond = 2.417f;
  static constexpr float kRefractionRateIce = 1.31f;

  explicit Dielectric(const float refractionRate)
      : m_refractionRate(refractionRate) {}

  [[nodiscard]]
  bool scatter(const Ray& rIn, const HitRecord& rec, glm::vec3& attenuation,
               Ray& scattered) const override;

  [[nodiscard]]
  std::unique_ptr<Material> clone() const override;

 private:
  [[nodiscard]]
  static float reflectance(const float cosTheta, const float refractionRate);

 private:
  float m_refractionRate;
};
}  // namespace mp