#include "Sphere.hpp"

#include "Ray.hpp"
#include "glm/glm.hpp"

namespace mp {
bool Hit(const Sphere& sphere, const Ray& ray, Interval<float> interval,
         HitRecord& hitRecord) {
  const auto oc = sphere.center - ray.origin();
  const float a = dot(ray.direction(), ray.direction());
  const auto h = dot(ray.direction(), oc);
  const float c = dot(oc, oc) - sphere.radius * sphere.radius;
  const auto desc = h * h - a * c;
  if (desc < 0.0f) {
    return false;
  }
  const auto sqrtd = std::sqrt(desc);
  auto root = (h - sqrtd) / a;
  if (!interval.surrounds(root)) {
    root = (h + sqrtd) / a;
    if (!interval.surrounds(root)) {
      return false;
    }
  }
  const auto sphereHit = ray.at(root);
  hitRecord.p = sphereHit;
  hitRecord.t = root;
  hitRecord.set_face_normal(ray, (sphereHit - sphere.center) / sphere.radius);
  hitRecord.mat = sphere.mat;
  return true;
}
}  // namespace mp