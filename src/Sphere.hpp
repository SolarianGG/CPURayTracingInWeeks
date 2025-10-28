#pragma once

#include "Hittable.hpp"

namespace mp {

struct Sphere {
  glm::vec3 center;
  float radius;
  std::shared_ptr<Material> mat;
};

[[nodiscard]]
bool Hit(const Sphere& sphere, const Ray& ray, Interval<float> interval,
         HitRecord& hitRecord);

}  // namespace mp
