#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ImageBuffer.hpp"
#include "Stb/stb_image_write.h"


namespace mp {
[[nodiscard]] bool save_png(const ImageBuffer& imageBuffer,
                           const std::string_view fileName) {
  return stbi_write_png(fileName.data(), imageBuffer.get_width(),
                        imageBuffer.get_height(), sizeof(Color),
                        imageBuffer.get_data(),
                        sizeof(Color) * imageBuffer.get_width());
}
}  // namespace mp