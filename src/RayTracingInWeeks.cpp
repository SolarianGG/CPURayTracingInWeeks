#include <algorithm>
#include <cstddef>
#include <format>
#include <iostream>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "Camera.hpp"
#include "ImageBuffer.hpp"
#include "Interval.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "glm/glm.hpp"

namespace rn = std::ranges;
namespace vi = std::views;

int main(int argc, char* argv[]) {
  using namespace mp;
  std::vector<Hittable> world;

  const auto materialGround =
      std::make_shared<Lambertian>(glm::vec3{0.5f, 0.5f, 0.5f});
  world.emplace_back(Sphere{{0.0f, -1000.0f, 0.0f}, 1000, materialGround});

  for (int a = -11; a < 11; ++a) {
    for (int b = -11; b < 11; ++b) {
      auto choose_mat = random_float();
      glm::vec3 center(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

      if ((center - glm::vec3(4, 0.2, 0)).length() > 0.9) {
        std::shared_ptr<Material> sphereMaterial;

        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = random_vec() * random_vec();
          sphereMaterial = std::make_shared<Lambertian>(albedo);
          world.emplace_back(Sphere(center, 0.2, sphereMaterial));
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = random_vec(0.5, 1);
          auto fuzz = random_float(0, 0.5);
          sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
          world.emplace_back(Sphere(center, 0.2, sphereMaterial));
        } else {
          // glass
          sphereMaterial = std::make_shared<Dielectric>(1.5);
          world.emplace_back(Sphere(center, 0.2, sphereMaterial));
        }
      }
    }
  }
  auto material1 = std::make_shared<Dielectric>(1.5);
  world.emplace_back(Sphere(glm::vec3(0, 1, 0), 1.0, material1));

  auto material2 = std::make_shared<Lambertian>(glm::vec3(0.4, 0.2, 0.1));
  world.emplace_back(Sphere(glm::vec3(-4, 1, 0), 1.0, material2));

  auto material3 = std::make_shared<Metal>(glm::vec3(0.7, 0.6, 0.5), 0.0);
  world.emplace_back(Sphere(glm::vec3(4, 1, 0), 1.0, material3));
  Camera camera{600,
                16.0 / 9.0,
                100,
                50,
                20.0f,
                0.2f,
                10.0f,
                glm::vec3{13.f, 2, 3},
                glm::vec3{0.0f, 0.0f, .0f}};
  return save_png(camera.render(world),
                  "results/materials_metal_nochecking.png") == 1
             ? EXIT_SUCCESS
             : EXIT_FAILURE;
}
