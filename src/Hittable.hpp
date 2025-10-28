#pragma once
#include <memory>
#include <utility>

#include "Interval.hpp"
#include "Ray.hpp"
#include "glm/glm.hpp"

namespace mp {

class Material;

struct HitRecord {
  glm::vec3 p;
  glm::vec3 normal;
  float t;
  bool frontFace;
  std::shared_ptr<Material> mat;

  void set_face_normal(const Ray& r, const glm::vec3& outwardNormal) {
    frontFace = dot(r.direction(), outwardNormal) < 0.0f;
    normal = frontFace ? outwardNormal : -outwardNormal;
  }
};

class Hittable final {
 private:
  struct IHittable {
    virtual ~IHittable() = default;
    [[nodiscard]]
    virtual std::unique_ptr<IHittable> copy() const = 0;
    virtual bool hit(const Ray& ray, Interval<float> interval,
                     HitRecord& hitRecord) const = 0;
  };

  template <typename T>
  struct HittableObject final : IHittable {
    T data;
    explicit HittableObject(T data) : data(std::move(data)) {}
    [[nodiscard]]
    std::unique_ptr<IHittable> copy() const override {
      return std::make_unique<HittableObject>(*this);
    }

    [[nodiscard]]
    bool hit(const Ray& ray, Interval<float> interval,
             HitRecord& hitRecord) const override {
      return Hit(data, ray, interval, hitRecord);
    }
  };

  std::unique_ptr<IHittable> m_self;

 public:
  template <typename T>
  explicit Hittable(T x)
      : m_self(std::make_unique<HittableObject<T>>(std::move(x))) {}

  [[nodiscard]]
  bool hit(const Ray& ray, Interval<float> interval,
           HitRecord& hitRecord) const {
    return m_self->hit(ray, interval, hitRecord);
  }

 public:
  Hittable(const Hittable& other) : m_self(other.m_self->copy()) {}
  Hittable(Hittable&& other) noexcept
      : m_self(std::exchange(other.m_self, nullptr)) {}
  Hittable& operator=(const Hittable& other) {
    m_self = other.m_self->copy();
    return *this;
  };
  Hittable& operator=(Hittable&& other) noexcept {
    m_self = std::move(other.m_self);
    return *this;
  };
  ~Hittable() = default;
};

}  // namespace mp