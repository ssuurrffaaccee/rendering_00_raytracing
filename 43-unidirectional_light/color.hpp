#pragma once
#include <iostream>

#include "interval.hpp"
#include "vec3.hpp"

using Color = Vec3;

double linear_to_gamma(double linear_component) {
  return sqrt(linear_component);
}

void write_color(std::ostream &out, Color pixel_color,size_t samples_per_pixel) {
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  // Divide the color by the number of samples.
  auto scale = 1.0 / samples_per_pixel;
  r *= scale;
  g *= scale;
  b *= scale;

  // Apply a linear to gamma transform for gamma 2
  r = linear_to_gamma(r);
  g = linear_to_gamma(g);
  b = linear_to_gamma(b);

  // Write the translated [0,255] value of each color component.
  static const Interval intensity(0.000, 0.999);
  out << int(256 * intensity.clamp(r)) << ' '
      << int(256 * intensity.clamp(g)) << ' '
      << int(256 * intensity.clamp(b)) << '\n';
}