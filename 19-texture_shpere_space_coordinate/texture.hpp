#pragma once
#include "color.hpp"
#include "vec3.hpp"
class Texture {
 public:
  virtual ~Texture() = default;

  virtual Color value(double u, double v, const Point3& p) const = 0;
};
class SolidColor : public Texture {
 public:
  SolidColor(const Color& c) : color_value_(c) {}

  SolidColor(double red, double green, double blue)
      : SolidColor{Color{red, green, blue}} {}

  Color value(double u, double v, const Point3& p) const override {
    return color_value_;
  }

 private:
  Color color_value_;
};
SPtr<Texture> make_solid_color_texture(const Color& c) {
  auto ptr = new SolidColor{c};
  return SPtr<Texture>{(Texture*)ptr};
}
SPtr<Texture> make_solid_color_texture(double red, double green, double blue) {
  auto ptr = new SolidColor{red, green, blue};
  return SPtr<Texture>{(Texture*)ptr};
}

class CheckerTexture : public Texture {
 public:
  CheckerTexture(double scale, const SPtr<Texture>& even,
                 const SPtr<Texture>& odd)
      : inv_scale_{1.0 / scale}, even_{even}, odd_{odd} {}

  CheckerTexture(double scale, const Color& c1, const Color& c2)
      : inv_scale_{1.0 / scale},
        even_(make_solid_color_texture(c1)),
        odd_(make_solid_color_texture(c2)) {}

  Color value(double u, double v, const Point3& p) const override {
    auto x_integer = static_cast<int>(std::floor(inv_scale_ * p.x()));
    auto y_integer = static_cast<int>(std::floor(inv_scale_ * p.y()));
    auto z_integer = static_cast<int>(std::floor(inv_scale_ * p.z()));

    bool is_even = (x_integer + y_integer + z_integer) % 2 == 0;

    return is_even ? even_->value(u, v, p) : odd_->value(u, v, p);
  }

 private:
  double inv_scale_;
  SPtr<Texture> even_;
  SPtr<Texture> odd_;
};

SPtr<Texture> make_checker_texture(double scale, const Color& c1,
                                   const Color& c2) {
  auto ptr = new CheckerTexture{scale, c1, c2};
  return SPtr<Texture>{(Texture*)ptr};
}