#pragma once
#include "hittable_list.hpp"
#include "instance.hpp"
#include "material.hpp"
#include "quad.hpp"
#include "sphere.hpp"
#include "texture.hpp"
inline SPtr<HittableList> box(const Point3& a, const Point3& b,
                              const SPtr<Material>& mat) {
  // Returns the 3D box (six sides) that contains the two opposite vertices a &
  // b.

  auto sides = make_shared<HittableList>();

  // Construct the two opposite vertices with the minimum and maximum
  // coordinates.
  auto min = Point3{fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z())};
  auto max = Point3{fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z())};

  auto dx = Vec3{max.x() - min.x(), 0, 0};
  auto dy = Vec3{0, max.y() - min.y(), 0};
  auto dz = Vec3{0, 0, max.z() - min.z()};

  sides->add(
      make_quad(Point3{min.x(), min.y(), max.z()}, dx, dy, mat));  // front
  sides->add(
      make_quad(Point3{max.x(), min.y(), max.z()}, -dz, dy, mat));  // right
  sides->add(
      make_quad(Point3{max.x(), min.y(), min.z()}, -dx, dy, mat));  // back
  sides->add(
      make_quad(Point3{min.x(), min.y(), min.z()}, dz, dy, mat));  // left
  sides->add(
      make_quad(Point3{min.x(), max.y(), max.z()}, dx, -dz, mat));  // top
  sides->add(
      make_quad(Point3{min.x(), min.y(), min.z()}, dx, dz, mat));  // bottom

  return sides;
}
SPtr<HittableList> make_cornell_box() {
  auto red = make_lambertian_material(Color{.65, .05, .05});
  auto white = make_lambertian_material(Color{.73, .73, .73});
  auto green = make_lambertian_material(Color{.12, .45, .15});
  auto light = make_diffuse_light_material(Color{15, 15, 15});
  auto world = make_shared<HittableList>();
  world->add(
      make_quad(Point3{555, 0, 0}, Vec3{0, 555, 0}, Vec3{0, 0, 555}, green));
  world->add(make_quad(Point3{0, 0, 0}, Vec3{0, 555, 0}, Vec3{0, 0, 555}, red));
  world->add(make_quad(Point3{343, 554, 332}, Vec3{-130, 0, 0},
                       Vec3{0, 0, -105}, light));
  world->add(
      make_quad(Point3{0, 0, 0}, Vec3{555, 0, 0}, Vec3{0, 0, 555}, white));
  world->add(make_quad(Point3{555, 555, 555}, Vec3{-555, 0, 0},
                       Vec3(0, 0, -555), white));
  world->add(
      make_quad(Point3{0, 0, 555}, Vec3{555, 0, 0}, Vec3{0, 555, 0}, white));
  SPtr<Hittable> box1 = box(Point3{0, 0, 0}, Point3{165, 330, 165}, white);
  box1 = make_rotate_y(box1, 15);
  box1 = make_translate(box1, Vec3{265, 0, 295});
  world->add(box1);

  SPtr<Hittable> box2 = box(Point3{0, 0, 0}, Point3{165, 165, 165}, white);
  box2 = make_rotate_y(box2, -18);
  box2 = make_translate(box2, Vec3{130, 0, 65});
  world->add(box2);
  return world;
}