#pragma once
#include "hittable.hpp"
#include "util.hpp"
#include "vec3.hpp"
class Sphere : public Hittable {
 public:
  Sphere(Point3 center, double radius) : center_{center}, radius_{radius} {}

  bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override {
    Vec3 oc = ray.origin() - center_;
    auto a = ray.direction().length_squared();
    auto half_b = dot(oc, ray.direction());
    auto c = oc.length_squared() - radius_ * radius_;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;

    // Find the nearest root that lies in the acceptable range.
    auto sqrtd = sqrt(discriminant);
    auto root = (-half_b - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
      root = (-half_b + sqrtd) / a;
      if (!ray_t.surrounds(root)) return false;
    }

    rec.t_ = root;
    rec.p_ = ray.at(rec.t_);
    Vec3 outward_normal = (rec.p_ - center_) / radius_;
    rec.set_face_normal(ray, outward_normal);
    return true;
  }

 private:
  Point3 center_;
  double radius_;
};

SPtr<Hittable> make_sphere(const Vec3& origin, const double radius) {
  auto ptr = new Sphere{origin, radius};
  return SPtr<Hittable>{(Hittable*)ptr};
}