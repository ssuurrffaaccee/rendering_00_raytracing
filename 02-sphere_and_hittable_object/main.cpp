#include "camera.hpp"
#include "check.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "image.hpp"
#include "sphere.hpp"
#include "thread_manager.hpp"
Color ray_color(const Ray &ray, const Hittable &world) {
  HitRecord rec;
  if (world.hit(ray, Interval{0, INFINITY_}, rec)) {
    return 0.5 * (rec.normal_ + Color(1, 1, 1));
  }
  Vec3 unit_direction = unit_vector(ray.direction());
  auto a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * Color{1.0, 1.0, 1.0} + a * Color{0.5, 0.7, 1.0};
}
int main() {
  try {
    Camera camera{};
    camera.image_width_ = 1000;
    camera.aspect_ratio_ = 16.0 / 9.0;
    camera.vfov_ = 90;
    camera.lookfrom_ = Point3(0, 0, 0);
    camera.lookat_ = Point3(0, 0, -1);
    camera.vup_ = Vec3(0, 1, 0);
    camera.initialize();
    auto [image_width, image_height] = camera.get_image_size();
    Image image{size_t(image_width), size_t(image_height)};
    int block_shape_on_x{50};
    int block_shape_on_y{50};
    auto ray_generators =
        camera.get_ray_generator(block_shape_on_x, block_shape_on_y);
    std::cout << "ray_generator num: " << ray_generators.size() << "\n";
    int thread_num = 5;
    ThreadManager td{thread_num};
    HittableList world;
    world.add(make_sphere(Point3(0, 0, -1), 0.5));
    world.add(make_sphere(Point3(0, -100.5, -1), 100));
    for (auto &ray_generator : ray_generators) {
      td.run([&]() {
        while (!ray_generator.is_finish()) {
          auto [ray, pixel_position] = ray_generator.next();
          Color color = ray_color(ray, world);
          image.set(pixel_position.i_, pixel_position.j_, color);
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