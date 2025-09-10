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

  const auto materialGround =
      std::make_shared<Lambertian>(glm::vec3{0.8f, 0.8f, 0.0f});
  const auto materialCenter =
      std::make_shared<Lambertian>(glm::vec3{0.1f, 0.2f, 0.5f});
  const auto materialLeft =
      std::make_shared<Dielectric>(Dielectric::kRefractionRateGlass);
  const auto materialBubble =
      std::make_shared<Dielectric>(1.0f / Dielectric::kRefractionRateGlass);
  const auto materialRight =
      std::make_shared<Metal>(glm::vec3{0.8f, 0.6f, 0.2f}, 0.8f);
  const std::vector world{
      Hittable{Sphere{.center = glm::vec3{0, -100.5f, -1.0f},
                      .radius = 100,
                      .mat = materialGround}},
      Hittable{Sphere{.center = glm::vec3{0.0f, 0.0f, -1.2f},
                      .radius = 0.5f,
                      .mat = materialCenter}},
      Hittable{Sphere{.center = glm::vec3{-1.0f, 0.0f, -1.0f},
                      .radius = 0.5f,
                      .mat = materialLeft}},
      Hittable{Sphere{.center = glm::vec3{-1.0f, 0.0f, -1.0f},
                      .radius = 0.4f,
                      .mat = materialBubble}},
      Hittable{Sphere{.center = glm::vec3{1.0f, 0.f, -1.0f},
                      .radius = 0.5f,
                      .mat = materialRight}}};

  mp::Camera camera{400, 16.0 / 9.0, 100, 50, 90.0f, glm::vec3{-2.0f, 2, 1}};
  return save_png(camera.render(world), "results/positionable_camera.png") == 1 ? EXIT_SUCCESS
                                                                 : EXIT_FAILURE;
}
