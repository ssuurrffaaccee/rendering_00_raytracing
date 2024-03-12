#pragma once
#include <assert.h>

#include <fstream>
#include <vector>

#include "check.hpp"
#include "color.hpp"
#include "vec3.hpp"
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