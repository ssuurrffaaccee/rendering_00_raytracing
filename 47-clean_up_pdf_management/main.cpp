#include "bvh_node.hpp"
#include "camera.hpp"
#include "check.hpp"
#include "cornell_box.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "image.hpp"
#include "material.hpp"
#include "pdf.hpp"
#include "quad.hpp"
#include "sphere.hpp"
#include "thread_manager.hpp"
#include "time.hpp"
// Color background_color(
//     const Ray &ray) {  // light is from here，background is the light source.
//   Vec3 unit_direction = unit_vector(ray.direction());
//   auto a = 0.5 * (unit_direction.y() + 1.0);
//   return (1.0 - a) * Color{1.0, 1.0, 1.0} + a * Color{0.5, 0.7, 1.0};
// }

// as/p(e+as/p(e+as/p(e))) = as/p*e + as/p*as/p*e + as/p*as/p*as/p*e
Color ray_color(const Ray &ray, const int max_depth, const Hittable &world,
                const Hittable &lights, const Color &background_color) {
  Ray cur_ray = ray;
  Color final_color{0.0f, 0.0f, 0.0f};
  // Color acc_attenuation{1.0f, 1.0f, 1.0f};
  Color acc_attenuation_times_scattering_over_sampling_pdf{1.0f, 1.0f, 1.0f};
  int depth = 0;
  std::vector<std::pair<Color, Color>> colors_;
  for (; depth < max_depth; depth++) {
    HitRecord rec;
    if (world.hit(cur_ray, Interval{0.001, INFINITY_}, rec)) {
      // Ray scattered;
      // Color attenuation;
      Color color_from_emission =
          rec.mat_->emitted(cur_ray, rec, rec.u_, rec.v_, rec.p_);
      ScatterRecord scatter_record;
      if (rec.mat_->scatter(cur_ray, rec, scatter_record)) { // ray path continue
        if (scatter_record.skip_pdf_) {
          acc_attenuation_times_scattering_over_sampling_pdf =
              acc_attenuation_times_scattering_over_sampling_pdf *
              scatter_record.attenuation_;
          cur_ray = scatter_record.skip_pdf_ray_;
          // no final_color += ..., just perfect specular, aka scattering_pdf=1
          // and nex_ray_sampling_pdf=1
          // and color_from_emission = Color{0, 0, 0};
          continue;
        }
        auto ligth_pdf_ptr = std::make_shared<HittablePDF>(lights, rec.p_);
        MixturePDF mixed_pdf{ligth_pdf_ptr,
                             scatter_record.next_ray_samplling_pdf_ptr_};
        Vec3 scatter_direction = mixed_pdf.sample();
        Ray scattered = Ray{rec.p_, scatter_direction, ray.time()};
        double nex_ray_sampling_pdf = mixed_pdf.pdf(scattered.direction());
        auto scattering_pdf = rec.mat_->scattering_pdf(ray, rec, scattered);
        final_color += acc_attenuation_times_scattering_over_sampling_pdf *
                       color_from_emission;
        acc_attenuation_times_scattering_over_sampling_pdf =
            acc_attenuation_times_scattering_over_sampling_pdf *
            (scatter_record.attenuation_ * scattering_pdf /
             nex_ray_sampling_pdf);
        cur_ray = scattered;
        continue;
      } else { // ray path finish
        final_color += acc_attenuation_times_scattering_over_sampling_pdf *
                       color_from_emission;
        break;
      }
    } else { // ray path finish
      final_color += acc_attenuation_times_scattering_over_sampling_pdf *
                     background_color; // light from background
      break;
    }
  }
  if (depth >= max_depth) {
    return Color{0, 0, 0};
  }
  return final_color;
}
void render(HittableList &world, const Hittable &lights, Camera &camera,
            Image &image, const int max_depth, int block_shape_on_x,
            int block_shape_on_y, int thread_num, bool using_bvh,
            const Color &background_color) {
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
  auto block_num = ray_generators.size();
  for (int i = 0; i < ray_generators.size(); i++) {
    auto &ray_generator = ray_generators[i];
    td.run([&, i]() {
      while (!ray_generator.is_finish()) {
        auto [ray, pixel_position] = ray_generator.next();
        Color color =
            ray_color(ray, max_depth, world, lights, background_color);
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
HittableList final_scene() {
  HittableList boxes1;
  { // ground
    auto ground = make_lambertian_material(Color{0.48, 0.83, 0.53});
    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
      for (int j = 0; j < boxes_per_side; j++) {
        auto w = 100.0;
        auto x0 = -1000.0 + i * w;
        auto z0 = -1000.0 + j * w;
        auto y0 = 0.0;
        auto x1 = x0 + w;
        auto y1 = random_double(1, 101);
        auto z1 = z0 + w;
        boxes1.add(box(Point3{x0, y0, z0}, Point3{x1, y1, z1}, ground));
      }
    }
  }

  HittableList world;
  world.add(make_bvh_node(boxes1));
  { // light
    auto light = make_diffuse_light_material(Color{7, 7, 7});
    world.add(make_quad(Point3{123, 554, 147}, Vec3{300, 0, 0}, Vec3{0, 0, 265},
                        light));
  }
  { // motion blur
    auto center1 = Point3{400, 400, 200};
    auto center2 = center1 + Vec3{30, 0, 0};
    auto sphere_material = make_lambertian_material(Color{0.7, 0.3, 0.1});
    world.add(make_moving_sphere(center1, center2, 50, sphere_material));
  }
  // dielectric
  world.add(make_static_sphere(Point3{260, 150, 45}, 50,
                               make_dielectric_material(1.5)));
  // metal
  world.add(make_static_sphere(Point3{0, 150, 145}, 50,
                               make_metal_material(Color{0.8, 0.8, 0.9}, 1.0)));

  // volume
  {
    auto boundary = make_static_sphere(Point3{360, 150, 145}, 70,
                                       make_dielectric_material(1.5));
    world.add(boundary);
    world.add(make_constant_medium(boundary, 0.2, Color{0.2, 0.4, 0.9}));
    boundary = make_static_sphere(Point3{0, 0, 0}, 5000,
                                  make_dielectric_material(1.5));
    world.add(make_constant_medium(boundary, .0001, Color{1, 1, 1}));
  }
  // texture
  auto emat = make_lambertian_material(
      make_image_texture("../../resources/earthmap.jpg"));
  world.add(make_static_sphere(Point3{400, 200, 400}, 100, emat));
  // perlin noise
  auto pertext = make_noise_texture(0.1);
  world.add(make_static_sphere(Point3{220, 280, 300}, 80,
                               make_lambertian_material(pertext)));

  // instance
  HittableList boxes2;
  auto white = make_lambertian_material(Color{.73, .73, .73});
  int ns = 1000;
  for (int j = 0; j < ns; j++) {
    boxes2.add(make_static_sphere(Point3::random(0, 165), 10, white));
  }

  world.add(make_translate(make_rotate_y(make_bvh_node(boxes2), 15),
                           Vec3{-100, 270, 395}));
  return world;
}
int main() {
  try {
    int samples_sqrt_per_pixel = 10;
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
      auto [world_ptr, lights_ptr] = make_cornell_box_aluminum();
      // world.add(make_cornell_box());
      bool using_bvh = false;
      Color backgroud_color = Color{0.00, 0.00, 0.00};
      render(*(world_ptr.get()), *(lights_ptr.get()), camera, image, max_depth,
             block_shape_on_x, block_shape_on_y, thread_num, using_bvh,
             backgroud_color);
    }
  } catch (MyExceptoin &e) {
    std::cout << e.what() << "\n";
  } catch (std::exception &e) {
    std::cout << e.what() << "\n";
  }
  return 0;
}
