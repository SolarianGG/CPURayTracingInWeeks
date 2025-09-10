#pragma once
#include <algorithm>
#include <cinttypes>
#include <format>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <utility>

#include "Color.hpp"
#include "Interval.hpp"
#include "glm/glm.hpp"

namespace mp {

class ImageBuffer {
 public:
  using image_dimensions_t = size_t;
  constexpr explicit ImageBuffer(const image_dimensions_t width,
                                 const image_dimensions_t height,
                                 const Color baseColor = Color::black())
      : m_width(width),
        m_height(height),
        m_pixels(std::make_unique<Color[]>(m_width * m_height)) {
    std::ranges::fill_n(m_pixels.get(), m_width * m_height, baseColor);
  }

  constexpr explicit ImageBuffer(const image_dimensions_t width,
                                 const double aspectRatio,
                                 const Color baseColor = Color::black())
      : ImageBuffer(
            width,
            glm::max(static_cast<image_dimensions_t>(1),
                     static_cast<image_dimensions_t>(width / aspectRatio)),
            baseColor) {}

  [[nodiscard]]
  const std::byte* get_data() const& {
    return reinterpret_cast<const std::byte*>(m_pixels.get());
  }

  [[nodiscard]]
  constexpr image_dimensions_t get_width() const noexcept {
    return m_width;
  }

  [[nodiscard]]
  constexpr image_dimensions_t get_height() const noexcept {
    return m_height;
  }

  [[nodiscard]]
  constexpr double ratio() const noexcept {
    return m_width / static_cast<double>(m_height);
  }

  template <typename Self>
  constexpr decltype(auto) at(this Self&& self, const image_dimensions_t x,
                              const image_dimensions_t y) {
    if (x >= m_width || y >= m_height) {
      throw std::out_of_range(
          std::format("Either x: {} or y: {} is out of range, image has width: "
                      "{} and height: {}",
                      x, y, m_width, m_height));
    }
    return std::forward_like<Self>(self.m_pixels[y * self.m_width + x]);
  }

  template <typename Self>
  constexpr decltype(auto) operator[](this Self&& self,
                                      const image_dimensions_t x,
                                      const image_dimensions_t y) {
    return std::forward_like<Self>(self.m_pixels[y * self.m_width + x]);
  }

 private:
  image_dimensions_t m_width;
  image_dimensions_t m_height;
  std::unique_ptr<Color[]> m_pixels;
};

[[nodiscard]] bool save_png(const ImageBuffer& imageBuffer,
                            const std::string_view fileName);
}  // namespace mp