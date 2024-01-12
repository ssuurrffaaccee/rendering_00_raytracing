#pragma once
#include "color.hpp"
#include "image.hpp"
#include "perlin.hpp"
#include "vec3.hpp"
class Texture {
 public:
  virtual ~Texture() = default;

  virtual Color value(double u, double v, const Point3 &p) const = 0;
};
class SolidColor : public Texture {
 public:
  SolidColor(const Color &c) : color_value_(c) {}

  SolidColor(double red, double green, double blue)
      : SolidColor{Color{red, green, blue}} {}

  Color value(double u, double v, const Point3 &p) const override {
    return color_value_;
  }

 private:
  Color color_value_;
};
SPtr<Texture> make_solid_color_texture(const Color &c) {
  auto ptr = new SolidColor{c};
  return SPtr<Texture>{(Texture *)ptr};
}
SPtr<Texture> make_solid_color_texture(double red, double green, double blue) {
  auto ptr = new SolidColor{red, green, blue};
  return SPtr<Texture>{(Texture *)ptr};
}

class CheckerTexture : public Texture {
 public:
  CheckerTexture(double scale, const SPtr<Texture> &even,
                 const SPtr<Texture> &odd)
      : inv_scale_{1.0 / scale}, even_{even}, odd_{odd} {}

  CheckerTexture(double scale, const Color &c1, const Color &c2)
      : inv_scale_{1.0 / scale},
        even_(make_solid_color_texture(c1)),
        odd_(make_solid_color_texture(c2)) {}

  Color value(double u, double v, const Point3 &p) const override {
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

SPtr<Texture> make_checker_texture(double scale, const Color &c1,
                                   const Color &c2) {
  auto ptr = new CheckerTexture{scale, c1, c2};
  return SPtr<Texture>{(Texture *)ptr};
}

class UVViewTexture : public Texture {
 public:
  UVViewTexture(double scale, const SPtr<Texture> &even,
                const SPtr<Texture> &odd)
      : scale_{scale}, even_{even}, odd_{odd} {}

  UVViewTexture(double scale, const Color &c1, const Color &c2)
      : scale_{scale},
        even_(make_solid_color_texture(c1)),
        odd_(make_solid_color_texture(c2)) {}

  Color value(double u, double v, const Point3 &p) const override {
    auto u_integer = int(scale_ * u);
    auto v_integer = int(scale_ * v);
    bool is_even = (u_integer + v_integer) % 2 == 0;
    return is_even ? even_->value(u, v, p) : odd_->value(u, v, p);
  }

 private:
  double scale_;
  SPtr<Texture> even_;
  SPtr<Texture> odd_;
};

SPtr<Texture> make_uv_view_texture(double scale, const Color &c1,
                                   const Color &c2) {
  auto ptr = new UVViewTexture{scale, c1, c2};
  return SPtr<Texture>{(Texture *)ptr};
}
Color mix(const Color &c0, const Color &c1, double r) {
  Color c_;
  c_.e[0] = (1.0 - r) * c0.e[0] + r * c1.e[0];
  c_.e[1] = (1.0 - r) * c0.e[1] + r * c1.e[1];
  c_.e[2] = (1.0 - r) * c0.e[2] + r * c1.e[2];
  return c_;
}
class ImageTexture : public Texture {
 public:
  ImageTexture(const std::string &filename) : image_{nullptr} {
    image_ = make_texture_image(filename);
  }
  Color value(double u, double v, const Point3 &p) const override {
    // If we have no texture data, then return solid cyan as a debugging aid.
    if (image_ == nullptr) {
      return Color{0, 1, 1};
    }
    // Clamp input texture coordinates to [0,1] x [1,0]
    u = Interval(0, 1).clamp(u);
    v = 1.0 - Interval(0, 1).clamp(v);  // Flip V to image coordinates
    auto i = int(u * image_->width_);
    auto j = int(v * image_->height_);
    return image_->get(i, j);
    // auto c0 = image_->get(i, j);
    // auto c1 = image_->get(i + 1, j);
    // auto c2 = image_->get(i, j + 1);
    // auto c3 = image_->get(i + 1, j + 1);
    // return mix(mix(c0, c1, u - int(u)), mix(c2, c3, u - int(u)), v - int(v));
  }

 private:
  SPtr<TexutreImage> image_;
};

SPtr<Texture> make_image_texture(const std::string &filename) {
  auto ptr = new ImageTexture{filename};
  return SPtr<Texture>{(Texture *)ptr};
}

class NoiseTexture : public Texture {
 public:
  NoiseTexture(double scale) : scale_{scale} {}

  Color value(double u, double v, const Point3 &p) const override {
    return Color(1, 1, 1) * noise.noise(scale_ * p);
  }

 private:
  Perlin noise;
  double scale_;
};

SPtr<Texture> make_noise_texture(double scale) {
  auto ptr = new NoiseTexture{scale};
  return SPtr<Texture>{(Texture *)ptr};
}