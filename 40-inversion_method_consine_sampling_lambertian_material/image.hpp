#pragma once
#include <assert.h>

#include <fstream>
#include <vector>

#include "check.hpp"
#include "color.hpp"
#include "vec3.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
/*
contract:
  +x = right direction
  +y = down direction
  width on x axis
  height on y axis
*/
class Image {
 public:
  Image(size_t width, size_t height, size_t smaple_per_pixel)
      : width_{width}, height_{height}, smaple_per_pixel_{smaple_per_pixel} {
    data_.resize(width_ * height_, Color{0.0f, 0.0f, 0.0f});
  }
  void set(size_t x, size_t y, const Color &color) {
    data_[y * width_ + x] = color;
  }
  void add(size_t x, size_t y, const Color &color) {
    data_[y * width_ + x] += color;
  }
  Color get(size_t x, size_t y) { return data_[y * width_ + x]; }
  void dump(const std::string name) {
    std::ofstream ofs(name + ".ppm");
    ofs << "P3\n" << width_ << ' ' << height_ << "\n255\n";
    for (auto &color : data_) {
      write_color(ofs, color, smaple_per_pixel_);
    }
  }
  size_t smaple_per_pixel_;
  size_t width_;
  size_t height_;
  std::vector<Color> data_;
};

/*
contract:
  +x = right direction
  +y = down direction
  +u = right direction
  +v = down diretion
  width on x axis, u axis
  height on y axis, v axis
*/
class TexutreImage {
 public:
  TexutreImage() {}
  TexutreImage(const std::string &path) { load(path); }
  void load(const std::string path) {
    int x, y, n;
    unsigned char *data = stbi_load(path.data(), &x, &y, &n, 0);
    CHECK(data != NULL)
    width_ = size_t(x);
    height_ = size_t(y);
    data_.resize(width_ * height_, Color{0.0f, 0.0f, 0.0f});
    for (int i = 0; i < x; i++) {
      for (int j = 0; j < y; j++) {
        Color color;
        for (int c = 0; c < n; c++) {
          color[c] = double(*(data + (j * x + i) * n + c)) / 255.0;
        }
        set(size_t(i), size_t(j), color);
      }
    }
    stbi_image_free(data);
  }
  void set(size_t x, size_t y, const Color &color) {
    data_[y * width_ + x] = color;
  }
  Color get(size_t x, size_t y) { return data_[y * width_ + x]; }
  // Color sample(double u, double v) {
  //   return get(u * (width_ - 1), v * (height_ - 1));
  // }
  size_t width_;
  size_t height_;
  std::vector<Color> data_;
};

std::shared_ptr<TexutreImage> make_texture_image(
    const std::string &image_path) {
  return std::make_shared<TexutreImage>(image_path);
}