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

