#pragma once
#include "aabb.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "vec3.hpp"
class Material;
class HitRecord {
public:
  Point3 p_;
  Vec3 normal_;
  double t_;
  bool front_face_;
  SPtr<Material> mat_;
  double u_;
  double v_;
  void set_face_normal(const Ray &ray, const Vec3 &outward_normal) {
    // Sets the hit record normal vector.
    // NOTE: the parameter `outward_normal` is assumed to have unit length.
    front_face_ = dot(ray.direction(), outward_normal) < 0;
    normal_ = front_face_ ? outward_normal : -outward_normal;
  }
};

class Hittable {
public:
  virtual ~Hittable() = default;

  virtual bool hit(const Ray &ray, Interval ray_t, HitRecord &rec) const = 0;
  virtual AABB bounding_box() const = 0;
  virtual std::string to_string() const = 0;
  virtual double pdf_from(const Point3 &o, const Vec3 &v) const { return 0.0; }
  virtual Vec3 sample_from(const Vec3 &o) const { return Vec3{1, 0, 0}; }
};