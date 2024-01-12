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
    scattered = Ray{rec.p_, direction, ray_in.time()};
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

    scattered = Ray{rec.p_, scatter_direction, r_in.time()};
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
class Metal : public Material {
 public:
  Metal(const Color& a, double fuzz) : albedo_{a}, fuzz_{fuzz} {}

  bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
               Ray& scattered) const override {
    Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal_);
    scattered = Ray{rec.p_, reflected + fuzz_ * random_unit_vector(),
                    r_in.time()};  // fuzz direction
    attenuation = albedo_;
    return (dot(scattered.direction(), rec.normal_) > 0);  // must in same side
    return true;
  }

 private:
  double fuzz_;
  Color albedo_;
};
SPtr<Material> make_metal_material(const Color& albedo, double fuzzy) {
  auto ptr = new Metal{albedo, fuzzy};
  return SPtr<Material>{(Material*)ptr};
}

class Dielectric : public Material {
 public:
  Dielectric(double index_of_refraction) : ir_{index_of_refraction} {}

  bool scatter(const Ray& ray_in, const HitRecord& rec, Color& attenuation,
               Ray& scattered) const override {
    attenuation = Color{1.0, 1.0, 1.0};
    double refraction_ratio = rec.front_face_ ? (1.0 / ir_) : ir_;
    Vec3 unit_direction = unit_vector(ray_in.direction());
    Vec3 refracted = refract(unit_direction, rec.normal_, refraction_ratio);
    double cos_theta =
        fmin(dot(-unit_direction, rec.normal_), 1.0);  // for incident angle
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    bool cannot_refract = refraction_ratio * sin_theta > 1.0;  //
    Vec3 direction;
    if (cannot_refract ||
        reflectance(cos_theta, refraction_ratio) > random_double()) {
      direction = reflect(unit_direction, rec.normal_);
    } else {
      direction = refract(unit_direction, rec.normal_, refraction_ratio);
    }
    scattered = Ray{rec.p_, refracted, ray_in.time()};
    return true;
  }

 private:
  double ir_;  // Index of Refraction
  static double reflectance(double cosine, double ref_idx) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
  }
};
SPtr<Material> make_dielectric_material(double index_of_refraction) {
  auto ptr = new Dielectric{index_of_refraction};
  return SPtr<Material>{(Material*)ptr};
}