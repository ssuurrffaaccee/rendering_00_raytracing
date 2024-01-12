#pragma once
#include "interval.hpp"
#include "ray.hpp"
#include "vec3.hpp"
class AABB {
 public:
  Interval x, y, z;

  AABB() {}  // The default AABB is empty, since Intervals are empty by default.

  AABB(const Interval& ix, const Interval& iy, const Interval& iz)
      : x(ix), y(iy), z(iz) {}

  AABB(const Point3& a, const Point3& b) {
    // Treat the two points a and b as extrema for the bounding box, so we don't
    // require a particular minimum/maximum coordinate order.
    x = Interval(fmin(a[0], b[0]), fmax(a[0], b[0]));
    y = Interval(fmin(a[1], b[1]), fmax(a[1], b[1]));
    z = Interval(fmin(a[2], b[2]), fmax(a[2], b[2]));
  }

  AABB(const AABB& box0, const AABB& box1) {
    x = merge(box0.x, box1.x);
    y = merge(box0.y, box1.y);
    z = merge(box0.z, box1.z);
  }

  AABB pad() {
    // Return an AABB that has no side narrower than some delta, padding if
    // necessary.
    double delta = 0.0001;
    Interval new_x = (x.size() >= delta) ? x : x.expand(delta);
    Interval new_y = (y.size() >= delta) ? y : y.expand(delta);
    Interval new_z = (z.size() >= delta) ? z : z.expand(delta);

    return AABB(new_x, new_y, new_z);
  }

  const Interval& axis(int n) const {
    if (n == 1) return y;
    if (n == 2) return z;
    return x;
  }

  bool hit(const Ray& r, Interval ray_t) const {
    for (int a = 0; a < 3; a++) {
      auto invD = 1 / r.direction()[a];
      auto orig = r.origin()[a];

      auto t0 = (axis(a).min - orig) * invD;
      auto t1 = (axis(a).max - orig) * invD;

      if (invD < 0) std::swap(t0, t1);

      if (t0 > ray_t.min) ray_t.min = t0;
      if (t1 < ray_t.max) ray_t.max = t1;

      if (ray_t.max <= ray_t.min) return false;
    }
    return true;
  }
};
AABB merge(const AABB& box1, const AABB& box2) {
  return AABB{merge(box1.x, box2.x), merge(box1.y, box2.y),
              merge(box1.z, box2.z)};
}
AABB operator+(const AABB& bbox, const Vec3& offset) {
  return AABB(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

AABB operator+(const Vec3& offset, const AABB& bbox) { return bbox + offset; }
