#pragma once
#include "vec3.hpp"

class Ray {
 public:
  Ray() {}

  Ray(const Point3& origin, const Vec3& direction)
      : orig_{origin}, dir_{direction} {}

  Point3 origin() const { return orig_; }
  Vec3 direction() const { return dir_; }

  Point3 at(double t) const { return orig_ + t * dir_; }

 private:
  Point3 orig_;
  Vec3 dir_;
};