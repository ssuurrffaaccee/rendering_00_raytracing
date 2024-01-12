#pragma once
#include "vec3.hpp"
// Orthonormal Basis
class OrthonormalBasis {
public:
  OrthonormalBasis() {}

  Vec3 operator[](int i) const { return axis_[i]; }
  Vec3 &operator[](int i) { return axis_[i]; }

  Vec3 u() const { return axis_[0]; }
  Vec3 v() const { return axis_[1]; }
  Vec3 w() const { return axis_[2]; }

  Vec3 local(double a, double b, double c) const {
    return a * u() + b * v() + c * w();
  }

  Vec3 local(const Vec3 &a) const {
    return a.x() * u() + a.y() * v() + a.z() * w();
  }

  void build_from_w(const Vec3 &w) {
    Vec3 unit_w = unit_vector(w);
    Vec3 a = (fabs(unit_w.x()) > 0.9) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
    Vec3 v = unit_vector(cross(unit_w, a));
    Vec3 u = cross(unit_w, v);
    axis_[0] = u;
    axis_[1] = v;
    axis_[2] = unit_w;
  }

public:
  Vec3 axis_[3];
};