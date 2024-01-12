#pragma once
#include "aabb.hpp"
#include "hittable.hpp"
#include "util.hpp"
#include "vec3.hpp"
class Quad : public Hittable {
public:
  Quad(const Point3 &plane_origin, const Vec3 &u, const Vec3 &v,
       const SPtr<Material> &mat)
      : plane_origin_{plane_origin}, u_{u}, v_{v}, mat_{mat} {
    set_bounding_box();
    // Finding the Plane That Contains a Given Quadrilateral
    auto n = cross(u, v);
    normal_ = unit_vector(n);
    D_ = dot(normal_, plane_origin);

    area_ = n.length();

    // Orienting Points on The Plane
    w_ = n / dot(n, n); // a lot of math!!!!
  }

  void set_bounding_box() {
    bbox_ = AABB(plane_origin_, plane_origin_ + u_ + v_).pad();
  }

  AABB bounding_box() const override { return bbox_; }
  std::string to_string() const override { return "quad"; }
  bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override {
    auto denom = dot(normal_, r.direction());

    // No hit if the ray is parallel to the plane.
    if (fabs(denom) < 1e-8)
      return false;

    // Return false if the hit point parameter t is outside the ray interval.
    auto t = (D_ - dot(normal_, r.origin())) / denom;
    if (!ray_t.contains(t))
      return false;
    // Determine the hit point lies within the planar shape using its plane
    // coordinates.
    auto intersection = r.at(t);
    Vec3 planar_hitpt_vector = intersection - plane_origin_;
    auto alpha = dot(w_, cross(planar_hitpt_vector, v_));
    auto beta = dot(w_, cross(u_, planar_hitpt_vector));

    if (!is_interior(alpha, beta, rec))
      return false;

    // Ray hits the 2D shape; set the rest of the hit record and return true.
    rec.t_ = t;
    rec.p_ = intersection;
    rec.mat_ = mat_;
    rec.set_face_normal(r, normal_);

    return true;
  }
  double pdf_from(const Point3 &origin,
                  const Vec3 &sample_direction) const override {
    HitRecord rec;
    // check if it is visible
    if (!this->hit(Ray{origin, sample_direction}, Interval{0.001, INFINITY_},
                   rec)) {
      return 0;
    }
    auto distance_squared = rec.t_ * rec.t_ * sample_direction.length_squared();
    auto cosine =
        fabs(dot(sample_direction, rec.normal_) / sample_direction.length());

    return distance_squared / (cosine * area_);
  }
  Vec3 sample_from(const Vec3 &origin) const override {
    auto p = plane_origin_ + (random_double() * u_) + (random_double() * v_);
    return p - origin;
  }

private:
  bool is_interior(double a, double b, HitRecord &rec) const {
    // Given the hit point in plane coordinates, return false if it is outside
    // the primitive, otherwise set the hit record UV coordinates and return
    // true.

    if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
      return false;

    rec.u_ = a;
    rec.v_ = b;
    return true;
  }
  Point3 plane_origin_;
  Vec3 u_, v_;
  SPtr<Material> mat_;
  AABB bbox_;
  Vec3 normal_;
  double D_;
  Vec3 w_;
  double area_;
};

SPtr<Hittable> make_quad(const Point3 &Q, const Vec3 &u, const Vec3 &v,
                         const SPtr<Material> &material) {
  auto ptr = new Quad{Q, u, v, material};
  return SPtr<Hittable>{(Hittable *)ptr};
}