#pragma once
#include "hittable.hpp"
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

class HittablePDF : public PDF {
public:
  HittablePDF(const Hittable &object, const Point3 &origin)
      : object_{object}, origin_{origin} {}

  double pdf(const Vec3 &direction) const override {
    return object_.pdf_from(origin_, direction);
  }

  Sample sample() const override { return object_.sample_from(origin_); }

private:
  const Hittable &object_;
  Point3 origin_;
};
class MixturePDF : public PDF {
public:
  MixturePDF(const SPtr<PDF> &p0, const SPtr<PDF> &p1) : p0_{p0}, p1_{p1} {}

  double pdf(const Vec3 &direction) const override {
    return 0.5 * p0_->pdf(direction) + 0.5 * p1_->pdf(direction);
  }

  Sample sample() const override {
    if (random_double() < 0.5) {
      return p0_->sample();
    } else {
      return p1_->sample();
    }
  }

private:
  SPtr<PDF> p0_;
  SPtr<PDF> p1_;
};