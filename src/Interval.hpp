#pragma once
#include "Utility.hpp"

namespace mp {

template <typename T>
struct Interval {
  T min{static_cast<T>(+infinity_f)}, max{static_cast<T>(-infinity_f)};

  [[nodiscard]]
  constexpr T size() const {
    return max - min;
  }

  [[nodiscard]]
  constexpr bool contains(T x) const {
    return min <= x && x <= max;
  }

  [[nodiscard]]
  constexpr bool surrounds(T x) const {
    return min < x && x < max;
  }

  [[nodiscard]]
  constexpr T clamp(T x) const {
    if (x < min) return min;
    if (x > max) return max;
    return x;
  }

  static constexpr Interval empty() {
    return Interval(+infinity_f, -infinity_f);
  }
  static constexpr Interval universe() {
    return Interval(-infinity_f, +infinity_f);
  }
};

}  // namespace mp