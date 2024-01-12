#pragma once
#include <sstream>
#include <vector>

#include "check.hpp"
#include "ray.hpp"
#include "vec3.hpp"
class Camera;
std::vector<std::pair<int, int>> divide_to_block_range(int total_length,
                                                       int length_per_block) {
  CHECK(total_length >= 1)
  CHECK(length_per_block >= 1)
  int block_num = int(total_length / length_per_block);
  int tail_block_length = total_length - block_num * length_per_block;
  std::vector<std::pair<int, int>> intervals;
  intervals.reserve(block_num + 1);
  for (int i = 0; i < block_num; i++) {
    int interval_base = i * length_per_block;
    intervals.push_back(
        std::make_pair(interval_base, interval_base + length_per_block));
  }
  if (tail_block_length != 0) {
    intervals.push_back(
        std::make_pair(total_length - tail_block_length, total_length));
  }
  return intervals;
}
struct PixelPosition {
  int i_;
  int j_;
};
class RayGenerator {
 public:
  std::pair<Ray, PixelPosition> next() {
    auto next_ray = get_ray();
    if (cur_pixel_samples_record_ <= 0) {
      int block_length_on_x = x_end_ - x_start_;
      int new_i = i_ + 1;
      i_ = x_start_ + (new_i - x_start_) % block_length_on_x;
      j_ = j_ + (new_i - x_start_) / block_length_on_x;
      cur_pixel_samples_record_ = samples_per_pixel_ - 1;
    }
    cur_pixel_samples_record_--;
    return next_ray;
  }
  bool is_finish() {
    if (i_ >= x_start_ && i_ <= (x_end_ - 1) && j_ >= y_start_ &&
        j_ <= (y_end_ - 1)) {
      return false;
    }
    return true;
  }

  std::string to_string() {
    std::stringstream ss;
    ss << "Range(" << x_start_ << "," << x_end_ << "," << y_start_ << ","
       << y_end_ << ")";
    return ss.str();
  }
  friend class Camera;

 private:
  Vec3 pixel_sample_square() const {
    // Returns a random point in the square surrounding a pixel at the origin.
    auto px = -0.5 + random_double();
    auto py = -0.5 + random_double();
    return (px * pixel_delta_u_) + (py * pixel_delta_v_);
  }
  std::pair<Ray, PixelPosition> get_ray() {
    auto pixel_center =
        pixel00_loc_ + (i_ * pixel_delta_u_) + (j_ * pixel_delta_v_);
    auto pixel_sample = pixel_center + pixel_sample_square();
    auto ray_origin = center_;
    auto ray_direction = pixel_sample - ray_origin;
    auto next_pixel_position = PixelPosition{i_, j_};
    return std::make_pair(Ray{ray_origin, ray_direction}, next_pixel_position);
  }

 private:
  int x_start_, x_end_;
  int y_start_, y_end_;
  int i_, j_;
  int samples_per_pixel_;
  int cur_pixel_samples_record_;
  Point3 pixel00_loc_;  // Location of pixel 0, 0
  Vec3 pixel_delta_u_;  // Offset to pixel to the right
  Vec3 pixel_delta_v_;  // Offset to pixel below
  Point3 center_;       // Camera center
};
class Camera {
 public:
  double aspect_ratio_ = 1.0;  // Ratio of image width over height
  int image_width_ = 100;      // Rendered image width in pixel count
  // int max_depth_ = 10;          // Maximum number of ray bounces into scene
  int samples_per_pixel_ = 10;  // Count of random samples for each pixel

  double vfov_ = 90;                    // Vertical view angle (field of view)
  Point3 lookfrom_ = Point3(0, 0, -1);  // Point camera is looking from
  Point3 lookat_ = Point3(0, 0, 0);     // Point camera is looking at
  Vec3 vup_ = Vec3(0.0f, 1.0f, 0.0f);   // Camera-relative "up" direction


 private:
  int image_height_;    // Rendered image height
  Point3 center_;       // Camera center
  Point3 pixel00_loc_;  // Location of pixel 0, 0
  Vec3 pixel_delta_u_;  // Offset to pixel to the right
  Vec3 pixel_delta_v_;  // Offset to pixel below
  Vec3 u_, v_, w_;      // Camera frame basis vectors
 public:
  void initialize() {
    CHECK(image_width_ > 0)
    CHECK(aspect_ratio_ > 0.0);
    CHECK(samples_per_pixel_ >= 1);
    image_height_ = static_cast<int>(image_width_ / aspect_ratio_);
    image_height_ = (image_height_ < 1) ? 1 : image_height_;
    center_ = lookfrom_;
        // Determine viewport dimensions.
    auto focal_length = (lookfrom_ - lookat_).length();
    auto theta = degrees_to_radians(vfov_);
    auto h = tan(theta / 2);
    auto viewport_height = 2 * h * focal_length;
    auto viewport_width =
        viewport_height * (static_cast<double>(image_width_) / image_height_);

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    w_ = unit_vector(lookfrom_ - lookat_);
    u_ = unit_vector(cross(vup_, w_));
    v_ = cross(w_, u_);

    // Calculate the vectors across the horizontal and down the vertical
    // viewport edges.
    Vec3 viewport_u =
        viewport_width * u_;  // Vector across viewport horizontal edge
    Vec3 viewport_v =
        viewport_height * -v_;  // Vector down viewport vertical edge

    // Calculate the horizontal and vertical delta vectors to the next pixel.
    pixel_delta_u_ = viewport_u / image_width_;
    pixel_delta_v_ = viewport_v / image_height_;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left =
        center_ - (focal_length * w_) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc_ =
        viewport_upper_left + 0.5 * (pixel_delta_u_ + pixel_delta_v_);
  }
  std::pair<int, int> get_image_size() {
    return std::make_pair(image_width_, image_height_);
  }
  std::vector<RayGenerator> get_ray_generator(int block_shape_on_x,
                                              int block_shape_on_y) {
    CHECK(block_shape_on_x >= 1)
    CHECK(block_shape_on_y >= 1)
    auto x_intervals = divide_to_block_range(image_width_, block_shape_on_x);
    auto y_intervals = divide_to_block_range(image_height_, block_shape_on_y);
    std::vector<RayGenerator> ray_generators;
    ray_generators.reserve(x_intervals.size() * y_intervals.size());
    for (auto &x_interval : x_intervals) {
      for (auto &y_interval : y_intervals) {
        RayGenerator ray_generator;
        ray_generator.x_start_ = x_interval.first;
        ray_generator.x_end_ = x_interval.second;
        ray_generator.y_start_ = y_interval.first;
        ray_generator.y_end_ = y_interval.second;
        ray_generator.i_ = ray_generator.x_start_;
        ray_generator.j_ = ray_generator.y_start_;
        ray_generator.pixel00_loc_ = pixel00_loc_;
        ray_generator.pixel_delta_u_ = pixel_delta_u_;
        ray_generator.pixel_delta_v_ = pixel_delta_v_;
        ray_generator.center_ = center_;
        ray_generator.samples_per_pixel_ = samples_per_pixel_;
        ray_generator.cur_pixel_samples_record_ = samples_per_pixel_;
        ray_generators.push_back(ray_generator);
      }
    }
    return ray_generators;
  }
};