#pragma once
#include "onb.hpp"
#include "vec3.hpp"
class PDF {
public:
  using Sample = Vec3;
  virtual Sample sample() const = 0;
  virtual double pdf(const Sample &x) const = 0;
};

class UniformSpherePDF : public PDF {
public:
  UniformSpherePDF() {}

  double pdf(const Sample &direction) const override { return 1 / (4 * PI); }

  Sample sample() const override { return random_unit_vector(); }
};

class CosineHemispherePDF : public PDF {
public:
  CosineHemispherePDF(const Vec3 &norm) : uvw_{} { uvw_.build_from_w(norm); }

  double pdf(const Sample &direction) const override {
    auto cosine_theta = dot(unit_vector(direction), uvw_.w());
    return fmax(0, cosine_theta / PI);
  }

  Sample sample() const override {
    return uvw_.local(random_cosine_direction());
  }

private:
  OrthonormalBasis uvw_;
};