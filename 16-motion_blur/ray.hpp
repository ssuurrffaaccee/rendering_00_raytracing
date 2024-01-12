#pragma once
#include "vec3.hpp"

class Ray {
 public:
  Ray() {}

  Ray(const Point3& origin, const Vec3& direction)
      : orig_{origin}, dir_{direction}, tm_{0.0f} {}
  Ray(const Point3& origin, const Vec3& direction, double offset_time)
      : orig_{origin}, dir_{direction}, tm_{offset_time} {}
  Point3 origin() const { return orig_; }
  Vec3 direction() const { return dir_; }
  double time() const { return tm_; }
  Point3 at(double t) const { return orig_ + t * dir_; }

 private:
  Point3 orig_;
  Vec3 dir_;
  double tm_;
};