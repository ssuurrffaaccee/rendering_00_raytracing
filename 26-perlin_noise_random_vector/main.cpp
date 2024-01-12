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
    const Ray &ray) { // light is from here，background is the light source.
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
void render(HittableList &world, Camera &camera, Image &image,
            const int max_depth, int block_shape_on_x, int block_shape_on_y,
            int thread_num, bool using_bvh) {
  auto ray_generators =
      camera.get_ray_generator(block_shape_on_x, block_shape_on_y);
  std::cout << "ray_generator num: " << ray_generators.size() << "\n";
  ThreadManager td{thread_num};
  if (using_bvh) {
    TIME_BEGIN(BVH_BUILD)
    world = HittableList{make_bvh_node(world)};
    TIME_END(BVH_BUILD)
    // std::cout << world.to_string() << "\n";
  }
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
    int thread_num = 5;
    {
      HittableList world;
      auto pertext = make_noise_texture(4);
      world.add(make_static_sphere(Point3{0, -1000, 0}, 1000,
                                   make_lambertian_material(pertext)));
      world.add(make_static_sphere(Point3{0, 2, 0}, 2,
                                   make_lambertian_material(pertext)));
      bool using_bvh = false;
      render(world, camera, image, max_depth, block_shape_on_x,
             block_shape_on_y, thread_num, using_bvh);
    }
  } catch (MyExceptoin &e) {
    std::cout << e.what() << "\n";
  } catch (std::exception &e) {
    std::cout << e.what() << "\n";
  }
  return 0;
}
