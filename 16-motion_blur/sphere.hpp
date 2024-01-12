#pragma once
#include "hittable.hpp"
#include "util.hpp"
#include "vec3.hpp"
class Sphere : public Hittable {
 public:
  Sphere(Point3 center, double radius, const SPtr<Material>& mat)
      : center_{center}, radius_{radius}, mat_{mat}, is_moving_{false} {}
  Sphere(Point3 center0, Point3 center1, double radius,
         const SPtr<Material>& mat)
      : center_{center0},
        centor_move_vec_{center1 - center0},
        radius_{radius},
        mat_{mat},
        is_moving_{true} {}
  bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override {
    Point3 centor_on_time = is_moving_ ? sphere_center(ray.time()) : center_;
    Vec3 oc = ray.origin() - centor_on_time;
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
    Vec3 outward_normal = (rec.p_ - centor_on_time) / radius_;
    rec.set_face_normal(ray, outward_normal);
    rec.mat_ = mat_;
    return true;
  }

 private:
  Point3 center_;
  Point3 centor_move_vec_;
  double radius_;
  SPtr<Material> mat_;
  bool is_moving_;
  Point3 sphere_center(double time) const {  // time in [0,1];
    // Linearly interpolate from center1 to center2 according to time, where t=0
    // yields center1, and t=1 yields center2.
    return center_ + time * centor_move_vec_;
  }
};

SPtr<Hittable> make_static_sphere(const Point3& origin, const double radius,
                                  const SPtr<Material>& material) {
  auto ptr = new Sphere{origin, radius, material};
  return SPtr<Hittable>{(Hittable*)ptr};
}
SPtr<Hittable> make_moving_sphere(const Point3& centor0, const Point3& centor1,
                                  const double radius,
                                  const SPtr<Material>& material) {
  auto ptr = new Sphere{centor0, centor1, radius, material};
  return SPtr<Hittable>{(Hittable*)ptr};
}
