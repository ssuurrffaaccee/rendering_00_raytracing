
#include "camera.hpp"
#include "check.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "image.hpp"
#include "quad.hpp"
#include "thread_manager.hpp"
#include "time.hpp"
// Color background_color(
//     const Ray &ray) {  // light is from here，background is the light source.
//   Vec3 unit_direction = unit_vector(ray.direction());
//   auto a = 0.5 * (unit_direction.y() + 1.0);
//   return (1.0 - a) * Color{1.0, 1.0, 1.0} + a * Color{0.5, 0.7, 1.0};
// }
// Color ray_color(const Ray &ray, const int max_depth, const Hittable &world,
//                 const Color &background_color) {
//   Ray cur_ray = ray;
//   Color final_color{0.0f, 0.0f, 0.0f};
//   Color acc_attenuation{1.0f, 1.0f, 1.0f};
//   int depth = 0;
//   for (; depth < max_depth; depth++) {
//     HitRecord rec;
//     if (world.hit(cur_ray, Interval{0.001, INFINITY_}, rec)) {
//       Ray scattered;
//       Color attenuation;
//       Color color_from_emission = rec.mat_->emitted(rec.u_, rec.v_, rec.p_);
//       if (rec.mat_->scatter(cur_ray, rec, attenuation,
//                             scattered)) { // ray path continue
//         final_color += acc_attenuation * color_from_emission;
//         acc_attenuation = acc_attenuation * attenuation;
//         cur_ray = scattered;
//         continue;
//       } else { // ray path finish
//         final_color += acc_attenuation * color_from_emission;
//         break;
//       }
//     } else { // ray path finish
//       final_color +=
//           acc_attenuation * background_color; // light from background
//       break;
//     }
//   }
//   if (depth >= max_depth) {
//     return Color{0, 0, 0};
//   }
//   return final_color;
// }
Color fragment_shader(int i, int j, int image_width, int image_height) {
  auto u = double(i) / image_width;
  auto v = double(j) / image_height;
  u = u - 0.5;
  v = v - 0.5;
  auto l = u * u + v * v;
  auto gray = fabs(sin(10000 * l));
  return Color{gray, gray, gray};
}
void render(HittableList &world, Camera &camera, Image &image,
            const int max_depth, int block_shape_on_x, int block_shape_on_y,
            int thread_num, const Color &background_color) {
  auto ray_generators =
      camera.get_ray_generator(block_shape_on_x, block_shape_on_y);
  std::cout << "ray_generator num: " << ray_generators.size() << "\n";
  ThreadManager td{thread_num};
  TIME_BEGIN(ALL_RENDERING)
  auto block_num = ray_generators.size();
  int image_width,image_height;
  std::tie(image_width, image_height) = camera.get_image_size();
  for (int i = 0; i < ray_generators.size(); i++) {
    auto &ray_generator = ray_generators[i];
    td.run([&, i]() {
      while (!ray_generator.is_finish()) {
        auto [ray, pixel_position] = ray_generator.next();
        auto color = fragment_shader(pixel_position.i_, pixel_position.j_,
                                     image_width, image_height);
        image.add(pixel_position.i_, pixel_position.j_, color);
      }
      double percentage = int((i / float(block_num)) * 100) / 100.0f;
      std::cout << "block " << i << " finish!! percentage: " << percentage
                << "\n";
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
    int samples_sqrt_per_pixel = 1;
    int max_depth = 50;
    CHECK(samples_sqrt_per_pixel >= 1);
    CHECK(max_depth >= 1);
    Camera camera{};
    camera.image_width_ = 600;
    camera.aspect_ratio_ = 1.0; // 16.0 / 9.0;
    camera.vfov_ = 40;
    camera.lookfrom_ = Point3{278, 278, -800};
    camera.lookat_ = Point3{278, 278, 0};
    camera.vup_ = Vec3{0, 1, 0};
    camera.defocus_angle_ = 0.0;
    camera.focus_dist_ = 10.0;
    camera.samples_sqrt_per_pixel_ = samples_sqrt_per_pixel;
    camera.initialize();
    auto [image_width, image_height] = camera.get_image_size();
    std::cout << "image shape " << image_width << "," << image_height << "\n";
    Image image{size_t(image_width), size_t(image_height),
                size_t(samples_sqrt_per_pixel * samples_sqrt_per_pixel)};
    int block_shape_on_x{50};
    int block_shape_on_y{50};
    int thread_num = 5;
    {
      HittableList world;
      // world.add(make_cornell_box());
      Color backgroud_color = Color{0.00, 0.00, 0.00};
      render(world, camera, image, max_depth, block_shape_on_x,
             block_shape_on_y, thread_num, backgroud_color);
    }
  } catch (MyExceptoin &e) {
    std::cout << e.what() << "\n";
  } catch (std::exception &e) {
    std::cout << e.what() << "\n";
  }
  return 0;
}
