#pragma once
#include "color.hpp"
#include "hittable.hpp"
#include "ray.hpp"
#include "vec3.hpp"
class Material {
 public:
  virtual ~Material() = default;

  virtual bool scatter(const Ray& ray_in, const HitRecord& rec,
                       Color& attenuation, Ray& scattered) const = 0;
};

class SimpleDiffuseMaterial : public Material {
 public:
  bool scatter(const Ray& ray_in, const HitRecord& rec, Color& attenuation,
               Ray& scattered) const override {
    Vec3 direction = random_on_hemisphere(rec.normal_);
    scattered = Ray{rec.p_, direction};
    attenuation = Color{0.5f, 0.5f, 0.5f};
    return true;
  }
};

SPtr<Material> make_simple_diffuse_material() {
  auto ptr = new SimpleDiffuseMaterial{};
  return SPtr<Material>{(Material*)ptr};
}

class Lambertian : public Material {
 public:
  Lambertian(const Color& a) : albedo_{a} {}

  bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
               Ray& scattered) const override {
    auto scatter_direction =
        rec.normal_ +
        random_unit_vector();  // oppsite may make scatter_direction zero

    // Catch degenerate scatter direction
    if (scatter_direction.near_zero()) {
      scatter_direction = rec.normal_;
    }

    scattered = Ray{rec.p_, scatter_direction};
    attenuation = albedo_;
    return true;
  }

 private:
  Color albedo_;
};

SPtr<Material> make_lambertian_material(const Color& albedo) {
  auto ptr = new Lambertian{albedo};
  return SPtr<Material>{(Material*)ptr};
}
