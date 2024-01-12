#pragma once
#include "color.hpp"
#include "hittable.hpp"
#include "onb.hpp"
#include "ray.hpp"
#include "texture.hpp"
#include "vec3.hpp"
class Material {
public:
  virtual ~Material() = default;
  virtual Color emitted(const Ray &r_in, const HitRecord &rec, double u,
                        double v, const Point3 &p) const {
    return Color{0, 0, 0};
  }
  virtual double next_ray_sampling_pdf(const Ray &r_in, const HitRecord &rec,
                                       const Ray &scattered) const {
    return 0;
  }
  virtual double scattering_pdf(const Ray &r_in, const HitRecord &rec,
                                const Ray &scattered) const {
    return 0;
  }
  virtual bool scatter(const Ray &ray_in, const HitRecord &rec,
                       Color &attenuation, Ray &scattered) const = 0;
};

class SimpleDiffuseMaterial : public Material {
public:
  bool scatter(const Ray &ray_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    Vec3 direction = random_on_hemisphere(rec.normal_);
    scattered = Ray{rec.p_, direction, ray_in.time()};
    attenuation = Color{0.5f, 0.5f, 0.5f};
    return true;
  }
};

SPtr<Material> make_simple_diffuse_material() {
  auto ptr = new SimpleDiffuseMaterial{};
  return SPtr<Material>{(Material *)ptr};
}

class Lambertian : public Material {
public:
  Lambertian(const Color &a) : albedo_{make_solid_color_texture(a)} {}
  Lambertian(const SPtr<Texture> &texture) : albedo_{texture} {}
  bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    OrthonormalBasis uvw;
    uvw.build_from_w(rec.normal_);
    // piared with function next_ray_sampling_pdf; and irrelated with
    // scattering_pdf
    auto scatter_direction = uvw.local(random_cosine_direction());
    scattered = Ray{rec.p_, scatter_direction, r_in.time()};
    attenuation = albedo_->value(rec.u_, rec.v_, rec.p_);
    return true;
  }
  double scattering_pdf(const Ray &r_in, const HitRecord &rec,
                        const Ray &scattered) const override {
    auto cos_theta = dot(rec.normal_, unit_vector(scattered.direction()));
    return cos_theta < 0 ? 0 : cos_theta / PI;
  }
  double next_ray_sampling_pdf(const Ray &r_in, const HitRecord &rec,
                               const Ray &scattered) const override {
    auto cos_theta = dot(rec.normal_, unit_vector(scattered.direction()));
    return cos_theta < 0 ? 0 : cos_theta / PI;
  }

private:
  SPtr<Texture> albedo_;
};
SPtr<Material> make_lambertian_material(const Color &albedo) {
  auto ptr = new Lambertian{albedo};
  return SPtr<Material>{(Material *)ptr};
}
SPtr<Material> make_lambertian_material(const SPtr<Texture> &texture) {
  auto ptr = new Lambertian{texture};
  return SPtr<Material>{(Material *)ptr};
}
class Metal : public Material {
public:
  Metal(const Color &a, double fuzz) : albedo_{a}, fuzz_{fuzz} {}

  bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal_);
    scattered = Ray{rec.p_, reflected + fuzz_ * random_unit_vector(),
                    r_in.time()}; // fuzz direction
    attenuation = albedo_;
    return (dot(scattered.direction(), rec.normal_) > 0); // must in same side
    return true;
  }

private:
  double fuzz_;
  Color albedo_;
};
SPtr<Material> make_metal_material(const Color &albedo, double fuzzy) {
  auto ptr = new Metal{albedo, fuzzy};
  return SPtr<Material>{(Material *)ptr};
}

class Dielectric : public Material {
public:
  Dielectric(double index_of_refraction) : ir_{index_of_refraction} {}

  bool scatter(const Ray &ray_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    attenuation = Color{1.0, 1.0, 1.0};
    double refraction_ratio = rec.front_face_ ? (1.0 / ir_) : ir_;
    Vec3 unit_direction = unit_vector(ray_in.direction());
    Vec3 refracted = refract(unit_direction, rec.normal_, refraction_ratio);
    double cos_theta =
        fmin(dot(-unit_direction, rec.normal_), 1.0); // for incident angle
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    bool cannot_refract = refraction_ratio * sin_theta > 1.0; //
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
  double ir_; // Index of Refraction
  static double reflectance(double cosine, double ref_idx) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
  }
};
SPtr<Material> make_dielectric_material(double index_of_refraction) {
  auto ptr = new Dielectric{index_of_refraction};
  return SPtr<Material>{(Material *)ptr};
}

class DiffuseLight : public Material {
public:
  DiffuseLight(const SPtr<Texture> &a) : emit(a) {}
  DiffuseLight(const Color &c) : emit(make_solid_color_texture(c)) {}

  bool scatter(const Ray &ray_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    return false;
  }

  Color emitted(const Ray &r_in, const HitRecord &rec, double u, double v,
                const Point3 &p) const override {
    if (!rec.front_face_) {
      return Color{0, 0, 0};
    }
    return emit->value(u, v, p);
  }

private:
  SPtr<Texture> emit;
};

SPtr<Material> make_diffuse_light_material(const Color &albedo) {
  auto ptr = new DiffuseLight{albedo};
  return SPtr<Material>{(Material *)ptr};
}
SPtr<Material> make_diffuse_light_material(const SPtr<Texture> &texture) {
  auto ptr = new DiffuseLight{texture};
  return SPtr<Material>{(Material *)ptr};
}
// uniform in all direction
class Isotropic : public Material {
public:
  Isotropic(const Color &c) : albedo_(make_solid_color_texture(c)) {}
  Isotropic(const SPtr<Texture> &a) : albedo_(a) {}

  bool scatter(const Ray &ray_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    // piared with function next_ray_sampling_pdf; and irrelated with
    // scattering_pdf
    auto scatter_direction = random_unit_vector();
    scattered = Ray{rec.p_, scatter_direction, ray_in.time()};
    attenuation = albedo_->value(rec.u_, rec.v_, rec.p_);
    return true;
  }
  double scattering_pdf(const Ray &r_in, const HitRecord &rec,
                        const Ray &scattered) const override {
    return 1 / (4 * PI);
  }
  double next_ray_sampling_pdf(const Ray &r_in, const HitRecord &rec,
                               const Ray &scattered) const override {
    auto cos_theta = dot(rec.normal_, unit_vector(scattered.direction()));
    return 1 / (4 * PI);
  }

private:
  SPtr<Texture> albedo_;
};

SPtr<Material> make_isotropic_material(const Color &albedo) {
  auto ptr = new Isotropic{albedo};
  return SPtr<Material>{(Material *)ptr};
}
SPtr<Material> make_isotropic_material(const SPtr<Texture> &texture) {
  auto ptr = new Isotropic{texture};
  return SPtr<Material>{(Material *)ptr};
}