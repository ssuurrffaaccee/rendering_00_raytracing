
#pragma once
#include "hittable.hpp"
#include "material.hpp"
#include "texture.hpp"
class ConstantMedium : public Hittable {
 public:
  ConstantMedium(const SPtr<Hittable>& b, double d, const SPtr<Texture>& a)
      : boundary_(b),
        neg_inv_density_(-1 / d),
        phase_function_(make_isotropic_material(a)) {}

  ConstantMedium(const SPtr<Hittable>& b, double d, const Color& c)
      : boundary_(b),
        neg_inv_density_(-1 / d),
        phase_function_(make_isotropic_material(c)) {}

  bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
    // Print occasional samples when debugging. To enable, set enableDebug true.
    // const bool enableDebug = false;
    // const bool debugging = enableDebug && random_double() < 0.00001;

    HitRecord rec1, rec2;

    // hit maybe near
    if (!boundary_->hit(r, Interval::universe, rec1)) return false;

    // hit maybe far
    if (!boundary_->hit(r, Interval(rec1.t_ + 0.0001, INFINITY_), rec2))
      return false;

    // if (debugging)
    //   std::clog << "\nray_tmin=" << rec1.t_ << ", ray_tmax=" << rec2.t <<
    //   '\n';
    // clamp
    if (rec1.t_ < ray_t.min) rec1.t_ = ray_t.min;
    if (rec2.t_ > ray_t.max) rec2.t_ = ray_t.max;
    if (rec1.t_ >= rec2.t_) return false;
    if (rec1.t_ < 0) rec1.t_ = 0;

    auto ray_length = r.direction().length();
    auto distance_inside_boundary = (rec2.t_ - rec1.t_) * ray_length;
    auto hit_distance =
        neg_inv_density_ * log(random_double());  // proceed random distance

    if (hit_distance > distance_inside_boundary) return false;

    rec.t_ = rec1.t_ + hit_distance / ray_length;
    rec.p_ = r.at(rec.t_);

    // if (debugging) {
    //   std::clog << "hit_distance = " << hit_distance << '\n'
    //             << "rec.t = " << rec.t << '\n'
    //             << "rec.p = " << rec.p << '\n';
    // }

    rec.normal_ = Vec3{1, 0, 0};  // arbitrary
    rec.front_face_ = true;       // also arbitrary
    rec.mat_ = phase_function_;

    return true;
  }

  AABB bounding_box() const override { return boundary_->bounding_box(); }
  std::string to_string() const override { return "constant_medium"; }

 private:
  SPtr<Hittable> boundary_;
  double neg_inv_density_;
  SPtr<Material> phase_function_;
};

SPtr<Hittable> make_constant_medium(const SPtr<Hittable>& b, double d,
                                    const Color& c) {
  auto ptr = new ConstantMedium{b, d, c};
  return SPtr<Hittable>{(Hittable*)ptr};
}
SPtr<Hittable> make_constant_medium(const SPtr<Hittable>& b, double d,
                                    const SPtr<Texture>& a) {
  auto ptr = new ConstantMedium{b, d, a};
  return SPtr<Hittable>{(Hittable*)ptr};
}