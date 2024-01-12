#include "camera.hpp"
#include "check.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "image.hpp"
#include "material.hpp"
#include "sphere.hpp"
#include "thread_manager.hpp"
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
  // std::vector<Vec3> path;
  // path.push_back(cur_ray.origin());
  for (; depth < max_depth; depth++) {
    HitRecord rec;
    if (world.hit(cur_ray, Interval{0.001, INFINITY_}, rec)) {
      Ray scattered;
      Color attenuation;
      if (rec.mat_->scatter(cur_ray, rec, attenuation, scattered)) {
        final_color = final_color * attenuation;
        cur_ray = scattered;
        // path.push_back(cur_ray.origin());
        continue;
      }
      final_color = final_color * Color{0, 0, 0};
      break;
    }
    // path.push_back(cur_ray.at(1));
    final_color = final_color * background_color(cur_ray);
    break;
  }
  if (depth >= max_depth) {
    return Color{0, 0, 0};
  }
  // std::cout<<"[";
  // for(auto&p:path){
  //   std::cout<<"["<<p<<"],";
  // }
  // std::cout<<"]\n";
  return final_color;
}
int main() {
  try {
    int samples_per_pixel = 100;
    int max_depth = 10;
    CHECK(samples_per_pixel >= 1);
    CHECK(max_depth >= 1);
    Camera camera{};
    camera.image_width_ = 1000;
    camera.aspect_ratio_ = 16.0 / 9.0;
    camera.vfov_ = 90;
    camera.lookfrom_ = Point3(0, 0, 0);
    camera.lookat_ = Point3(0, 0, -1);
    camera.vup_ = Vec3(0, 1, 0);
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
    auto material_ground = make_lambertian_material(Color(0.8, 0.8, 0.0));
    auto material_center = make_dielectric_material(1.5);
    auto material_left = make_dielectric_material(1.5);
    auto material_right = make_metal_material(Color(0.8, 0.6, 0.2), 1.0);

    world.add(make_sphere(Point3{0.0, -100.5, -1.0}, 100.0, material_ground));
    world.add(make_sphere(Point3{0.0, 0.0, -1.0}, 0.5, material_center));
    world.add(make_sphere(Point3{-1.0, 0.0, -1.0}, 0.5, material_left));
    world.add(make_sphere(Point3{1.0, 0.0, -1.0}, 0.5, material_right));
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
    image.dump("xxxx");
  } catch (MyExceptoin &e) {
    std::cout << e.what() << "\n";
  } catch (std::exception &e) {
    std::cout << e.what() << "\n";
  }
  return 0;
}