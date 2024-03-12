#pragma once
#include "constant_medium.hpp"
#include "hittable_list.hpp"
#include "instance.hpp"
#include "material.hpp"
#include "quad.hpp"
#include "sphere.hpp"
#include "texture.hpp"
#include "triangle.hpp"
#include "triangle_mesh.hpp"
inline SPtr<HittableList> box(const Point3 &a, const Point3 &b,
                              const SPtr<Material> &mat) {
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
SPtr<HittableList> pyramid(const Point3 &a, const Point3 &b,
                           const SPtr<Material> &mat,
                           const SPtr<Material> &triangle_mat) {
  auto sides = make_shared<HittableList>();

  // Construct the two opposite vertices with the minimum and maximum
  // coordinates.
  auto min = Point3{fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z())};
  auto max = Point3{fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z())};

  auto dx = Vec3{max.x() - min.x(), 0, 0};
  auto dy = Vec3{0, max.y() - min.y(), 0};
  auto dz = Vec3{0, 0, max.z() - min.z()};
  auto head = (a + b) * 0.5 + 0.5 * dy;
  auto bottom_0 = Point3{min.x(), min.y(), min.z()};
  auto bottom_1 = bottom_0 + dx;
  auto bottom_2 = bottom_0 + dz + dx;
  auto bottom_3 = bottom_0 + dz;
  sides->add(make_triangle(head, bottom_0, bottom_1, triangle_mat));  // front
  sides->add(make_triangle(head, bottom_1, bottom_2, triangle_mat));  // right
  sides->add(make_triangle(head, bottom_2, bottom_3, triangle_mat));  // back
  sides->add(make_triangle(head, bottom_3, bottom_0, triangle_mat));  // left
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

SPtr<HittableList> make_cornell_box_fog() {
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

  SPtr<Hittable> box2 = box(Point3{0, 0, 0}, Point3{165, 165, 165}, white);
  box2 = make_rotate_y(box2, -18);
  box2 = make_translate(box2, Vec3{130, 0, 65});

  world->add(make_constant_medium(box1, 0.01, Color{0, 0, 0}));
  world->add(make_constant_medium(box2, 0.01, Color{1, 1, 1}));
  return world;
}

std::pair<SPtr<HittableList>, SPtr<Hittable>>
make_cornell_box_with_explicit_lights() {
  auto red = make_lambertian_material(Color{.65, .05, .05});
  auto white = make_lambertian_material(Color{.73, .73, .73});
  auto green = make_lambertian_material(Color{.12, .45, .15});
  auto light = make_diffuse_light_material(Color{15, 15, 15});
  auto world = make_shared<HittableList>();
  //////////////////just for sampling
  auto m = std::shared_ptr<Material>();
  auto lights =
      make_quad(Point3{343, 554, 332}, Vec3{-130, 0, 0}, Vec3{0, 0, -105}, m);
  /////////////////
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
  return std::make_pair(world, lights);
}
std::pair<SPtr<HittableList>, SPtr<Hittable>> make_cornell_box_aluminum() {
  auto red = make_lambertian_material(Color{.65, .05, .05});
  auto white = make_lambertian_material(Color{.73, .73, .73});
  auto green = make_lambertian_material(Color{.12, .45, .15});
  auto light = make_diffuse_light_material(Color{15, 15, 15});
  auto world = make_shared<HittableList>();
  //////////////////just for sampling
  auto m = std::shared_ptr<Material>();
  auto lights =
      make_quad(Point3{343, 554, 332}, Vec3{-130, 0, 0}, Vec3{0, 0, -105}, m);
  /////////////////
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
  auto aluminum = make_metal_material(Color{0.8, 0.85, 0.88}, 0.0);
  SPtr<Hittable> box1 = box(Point3{0, 0, 0}, Point3{165, 330, 165}, aluminum);
  box1 = make_rotate_y(box1, 15);
  box1 = make_translate(box1, Vec3{265, 0, 295});
  world->add(box1);

  SPtr<Hittable> box2 = box(Point3{0, 0, 0}, Point3{165, 165, 165}, white);
  box2 = make_rotate_y(box2, -18);
  box2 = make_translate(box2, Vec3{130, 0, 65});
  world->add(box2);
  return std::make_pair(world, lights);
}

std::pair<SPtr<HittableList>, SPtr<Hittable>>
make_cornell_box_with_explicit_lights_plus_gloss_shpere() {
  auto red = make_lambertian_material(Color{.65, .05, .05});
  auto white = make_lambertian_material(Color{.73, .73, .73});
  auto green = make_lambertian_material(Color{.12, .45, .15});
  auto light = make_diffuse_light_material(Color{15, 15, 15});
  auto world = make_shared<HittableList>();
  //////////////////just for sampling
  auto lights = make_shared<HittableList>();
  auto m = std::shared_ptr<Material>();
  lights->add(
      make_quad(Point3{343, 554, 332}, Vec3{-130, 0, 0}, Vec3{0, 0, -105}, m));
  lights->add(make_static_sphere(Point3{190, 90, 190}, 90, m));
  /////////////////
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
  // Glass Sphere
  auto glass = make_dielectric_material(1.5);
  world->add(make_static_sphere(Point3{190, 90, 190}, 90, glass));
  //   SPtr<Hittable> box2 = box(Point3{0, 0, 0}, Point3{165, 165, 165}, white);
  //   box2 = make_rotate_y(box2, -18);
  //   box2 = make_translate(box2, Vec3{130, 0, 65});
  //   world->add(box2);
  return std::make_pair(world, lights);
}
std::pair<SPtr<HittableList>, SPtr<Hittable>>
make_cornell_box_with_explicit_lights_pyramid() {
  auto red = make_lambertian_material(Color{.65, .05, .05});
  auto white = make_lambertian_material(Color{.73, .73, .73});
  auto green = make_lambertian_material(Color{.12, .45, .15});
  auto light = make_diffuse_light_material(Color{15, 15, 15});
  auto world = make_shared<HittableList>();
  auto triangle_texture = make_triangle_interpolation_texture(
      Color{1.0f, 0.0f, 0.0f}, Color{0.0f, 1.0f, 0.0f},
      Color{0.0f, 0.0f, 1.0f});
  auto triangle_material = make_lambertian_material(triangle_texture);
  //////////////////just for sampling
  auto m = std::shared_ptr<Material>();
  auto lights =
      make_quad(Point3{343, 554, 332}, Vec3{-130, 0, 0}, Vec3{0, 0, -105}, m);
  /////////////////
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

  SPtr<Hittable> box2 =
      pyramid(Point3{0, 0, 0}, Point3{165, 165, 165}, white, triangle_material);
  box2 = make_rotate_y(box2, -18);
  box2 = make_translate(box2, Vec3{130, 0, 65});
  world->add(box2);
  return std::make_pair(world, lights);
}

std::pair<SPtr<HittableList>, SPtr<Hittable>>
make_cornell_box_with_explicit_lights_cow() {
  auto red = make_lambertian_material(Color{.65, .05, .05});
  auto white = make_lambertian_material(Color{.73, .73, .73});
  auto green = make_lambertian_material(Color{.12, .45, .15});
  auto light = make_diffuse_light_material(Color{15, 15, 15});
  auto world = make_shared<HittableList>();
  auto triangle_texture = make_triangle_interpolation_texture(
      Color{1.0f, 0.0f, 0.0f}, Color{0.0f, 1.0f, 0.0f},
      Color{0.0f, 0.0f, 1.0f});
  auto triangle_material = make_lambertian_material(triangle_texture);
  //////////////////just for sampling
  auto m = std::shared_ptr<Material>();
  auto lights =
      make_quad(Point3{343, 554, 332}, Vec3{-130, 0, 0}, Vec3{0, 0, -105}, m);
  /////////////////
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
//   SPtr<Hittable> box1 = box(Point3{0, 0, 0}, Point3{165, 330, 165}, white);
//   box1 = make_rotate_y(box1, 15);
//   box1 = make_translate(box1, Vec3{265, 0, 295});
//   world->add(box1);

//   SPtr<Hittable> box2 =
//       pyramid(Point3{0, 0, 0}, Point3{165, 165, 165}, white, triangle_material);
//   
//   box2 = make_translate(box2, Vec3{130, 0, 65});
//   world->add(box2);
  auto cow = build_cow_mesh();
  auto rotated_cow = make_rotate_y(cow, 130);
  auto translated_cow = make_translate(rotated_cow, Vec3{277, 0, 277});
  world->add(translated_cow);
  return std::make_pair(world, lights);
}