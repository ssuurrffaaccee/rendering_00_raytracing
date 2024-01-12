#include "bvh_node.hpp"
#include "camera.hpp"
#include "check.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "image.hpp"
#include "material.hpp"
#include "sphere.hpp"
#include "thread_manager.hpp"
#include "time.hpp"
Color background_color(
    const Ray &ray) {  // light is from here，background is the light source.
  Vec3 unit_direction = unit_vector(ray.direction());
  auto a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * Color{1.0, 1.0, 1.0} + a * Color{0.5, 0.7, 1.0};
}
Color ray_color(const Ray &ray, const int max_depth, const Hittable &world) {
  Ray cur_ray = ray;
  Color final_color{1.0f, 1.0f, 1.0f};
  int depth = 0;
  for (; depth < max_depth; depth++) {
    HitRecord rec;
    if (world.hit(cur_ray, Interval{0.001, INFINITY_}, rec)) {
      Ray scattered;
      Color attenuation;
      if (rec.mat_->scatter(cur_ray, rec, attenuation, scattered)) {
        final_color = final_color * attenuation;
        cur_ray = scattered;
        continue;
      }
      final_color = final_color * Color{0, 0, 0};  
      break;
    }
    final_color = final_color * background_color(cur_ray);
    break;
  }
  if (depth >= max_depth) {
    return Color{0, 0, 0};
  }
  return final_color;
}
int main() {
  try {
    int samples_per_pixel = 10;
    int max_depth = 10;
    CHECK(samples_per_pixel >= 1);
    CHECK(max_depth >= 1);
    Camera camera{};
    camera.image_width_ = 1200;
    camera.aspect_ratio_ = 16.0 / 9.0;
    camera.vfov_ = 20;
    camera.lookfrom_ = Point3{13, 2, 3};
    camera.lookat_ = Point3{0, 0, 0};
    camera.vup_ = Vec3{0, 1, 0};
    camera.defocus_angle_ = 0.0;
    camera.focus_dist_ = 10.0;
    camera.samples_per_pixel_ = samples_per_pixel;
    camera.initialize();
    auto [image_width, image_height] = camera.get_image_size();
    std::cout << "image shape " << image_width << "," << image_height << "\n";
    Image image{size_t(image_width), size_t(image_height),
                size_t(samples_per_pixel)};
    int block_shape_on_x{50};
    int block_shape_on_y{50};
    auto ray_generators =
        camera.get_ray_generator(block_shape_on_x, block_shape_on_y);
    std::cout << "ray_generator num: " << ray_generators.size() << "\n";
    int thread_num = 5;
    ThreadManager td{thread_num};
    HittableList world;
    {
      auto checker =
          make_checker_texture(0.32, Color{.2, .3, .1}, Color{.9, .9, .9});
      auto ground_material = make_lambertian_material(checker);
      world.add(make_static_sphere(Point3(0, -1000, 0), 1000, ground_material));

      for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
          auto choose_mat = random_double();
          Point3 center(a + 0.9 * random_double(), 0.2,
                        b + 0.9 * random_double());

          if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
            SPtr<Material> sphere_material;

            if (choose_mat < 0.8) {
              // diffuse
              auto albedo = Color::random() * Color::random();
              auto center1 = center + Vec3(0, random_double(0, .5), 0);
              sphere_material = make_lambertian_material(albedo);
              world.add(
                  make_moving_sphere(center, center1, 0.2, sphere_material));
            } else if (choose_mat < 0.95) {
              // metal
              auto albedo = Color::random(0.5, 1);
              auto fuzz = random_double(0, 0.5);
              sphere_material = make_metal_material(albedo, fuzz);
              world.add(make_static_sphere(center, 0.2, sphere_material));
            } else {
              // glass
              sphere_material = make_dielectric_material(1.5);
              world.add(make_static_sphere(center, 0.2, sphere_material));
            }
          }
        }
      }

      auto material1 = make_dielectric_material(1.5);
      world.add(make_static_sphere(Point3(0, 1, 0), 1.0, material1));

      auto material2 = make_lambertian_material(Color(0.4, 0.2, 0.1));
      world.add(make_static_sphere(Point3(-4, 1, 0), 1.0, material2));

      auto material3 = make_metal_material(Color(0.7, 0.6, 0.5), 0.0);
      world.add(make_static_sphere(Point3(4, 1, 0), 1.0, material3));
    }
    TIME_BEGIN(BVH_BUILD)
    world = HittableList{make_bvh_node(world)};
    TIME_END(BVH_BUILD)
    // std::cout << world.to_string() << "\n";
    TIME_BEGIN(ALL_RENDERING)
    for (auto &ray_generator : ray_generators) {
      td.run([&]() {
        while (!ray_generator.is_finish()) {
          auto [ray, pixel_position] = ray_generator.next();
          Color color = ray_color(ray, max_depth, world);
          image.add(pixel_position.i_, pixel_position.j_, color);
        }
      });
    }
    td.stop_when_finish_all();
    td.wait();
    TIME_END(ALL_RENDERING)
    TIME_BEGIN(DUMP_IMAGE)
    image.dump("xxxx");
    TIME_END(DUMP_IMAGE)
  } catch (MyExceptoin &e) {
    std::cout << e.what() << "\n";
  } catch (std::exception &e) {
    std::cout << e.what() << "\n";
  }
  return 0;
}