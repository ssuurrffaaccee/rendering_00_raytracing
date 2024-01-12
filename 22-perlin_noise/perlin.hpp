#pragma once
#include <vector>

#include "vec3.hpp"
class Perlin {
 public:
  Perlin() {
    ranfloat_.resize(point_count);
    for (int i = 0; i < point_count; ++i) {
      ranfloat_[i] = random_double();
    }

    perm_x_ = perlin_generate_perm();
    perm_y_ = perlin_generate_perm();
    perm_z_ = perlin_generate_perm();
  }
  double noise(const Point3 &p) const {
    auto i = static_cast<int>(4 * p.x()) & 255;
    auto j = static_cast<int>(4 * p.y()) & 255;
    auto k = static_cast<int>(4 * p.z()) & 255;

    return ranfloat_[perm_x_[i] ^ perm_y_[j] ^
                     perm_z_[k]];  // this is a hash function,map [R,R,R] to int, a.k.a map 3d volume to int
  }

 private:
  const int point_count = 256;
  std::vector<double> ranfloat_;
  std::vector<int> perm_x_;
  std::vector<int> perm_y_;
  std::vector<int> perm_z_;

  std::vector<int> perlin_generate_perm() {
    std::vector<int> p;
    p.resize(point_count);
    for (int i = 0; i < Perlin::point_count; i++) {
      p[i] = i;
    }
    permute(p);
    return p;
  }

  void permute(std::vector<int> &p) {
    for (int i = p.size() - 1; i > 0; i--) {
      int target = random_int(0, i);
      int tmp = p[i];
      p[i] = p[target];
      p[target] = tmp;
    }
  }
};