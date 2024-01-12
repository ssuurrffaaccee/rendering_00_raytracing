#pragma once
#include "aabb.hpp"
#include "hittable.hpp"
#include "util.hpp"
#include "vec3.hpp"
class Sphere : public Hittable {
 public:
  Sphere(Point3 center, double radius, const SPtr<Material>& mat)
      : center_{center}, radius_{radius}, mat_{mat}, is_moving_{false} {
    auto rvec = Vec3{radius, radius, radius};
    bbox_ = AABB(center_ - rvec, center_ + rvec);
  }
  Sphere(Point3 center0, Point3 center1, double radius,
         const SPtr<Material>& mat)
      : center_{center0},
        centor_move_vec_{center1 - center0},
        radius_{radius},
        mat_{mat},
        is_moving_{true} {
    auto rvec = Vec3{radius, radius, radius};
    AABB box1(center0 - rvec, center0 + rvec);
    AABB box2(center1 - rvec, center1 + rvec);
    bbox_ = merge(box1, box2);
  }
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
    auto [u, v] = get_sphere_uv(outward_normal);
    rec.u_ = u;
    rec.v_ = v;
    return true;
  }
  AABB bounding_box() const override { return bbox_; }
  std::string to_string() const override { return "sphere"; }

 private:
  Point3 center_;
  Point3 centor_move_vec_;
  double radius_;
  SPtr<Material> mat_;
  bool is_moving_;
  AABB bbox_;
  Point3 sphere_center(double time) const {  // time in [0,1];
    // Linearly interpolate from center1 to center2 according to time, where t=0
    // yields center1, and t=1 yields center2.
    return center_ + time * centor_move_vec_;
  }
  std::pair<double, double> get_sphere_uv(const Point3& p) const {
    // normal: a given point on the sphere of radius 1.0f, centered at the
    // origin. u: returned value [0,1] of angle around the Y axis from X=-1.
    // horizontal v: returned value [0,1] of angle from Y=-1 to Y=+1. vertical
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

    auto theta = acos(-p.y());             //[-1,1]->[0,PI]
    auto phi = atan2(-p.z(), p.x()) + PI;  //[R,R]->[-pi,pi];
    return std::make_pair(phi / (2 * PI), theta / PI);
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
