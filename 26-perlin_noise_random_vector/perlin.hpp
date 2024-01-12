#pragma once
#include <vector>

#include "vec3.hpp"
class Perlin {
 public:
  Perlin() {
    ranvec_.resize(point_count);
    for (int i = 0; i < point_count; ++i) {
      ranvec_[i] = unit_vector(Vec3::random(-1, 1));
    }

    perm_x_ = perlin_generate_perm();
    perm_y_ = perlin_generate_perm();
    perm_z_ = perlin_generate_perm();
  }
  double noise(const Point3 &p) const {
    // fraction part
    auto u = p.x() - floor(p.x());
    auto v = p.y() - floor(p.y());
    auto w = p.z() - floor(p.z());
    // interger part
    auto i = static_cast<int>(floor(p.x()));
    auto j = static_cast<int>(floor(p.y()));
    auto k = static_cast<int>(floor(p.z()));
    Vec3 c[2][2][2];  // neighbor in 3d space , p in cube center , c is cube's
                      // vertex

    for (int di = 0; di < 2; di++)
      for (int dj = 0; dj < 2; dj++)
        for (int dk = 0; dk < 2; dk++)
          c[di][dj][dk] =
              ranvec_[perm_x_[(i + di) & 255] ^ perm_y_[(j + dj) & 255] ^
                      perm_z_[(k + dk) & 255]];

    return perlin_interp(c, u, v, w);
  }

 private:
  const int point_count = 256;
  std::vector<Vec3> ranvec_;
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
  // interpolation in cube's internal
  double perlin_interp(Vec3 c[2][2][2], double u, double v, double w) const {
    auto uu = u * u * (3 - 2 * u);
    auto vv = v * v * (3 - 2 * v);
    auto ww = w * w * (3 - 2 * w);
    auto accum = 0.0;

    for (int i = 0; i < 2; i++)
      for (int j = 0; j < 2; j++)
        for (int k = 0; k < 2; k++) {
          Vec3 weight_v(u - i, v - j, w - k);  // distance vector
          accum += (i * uu + (1 - i) * (1 - uu)) *
                   (j * vv + (1 - j) * (1 - vv)) *
                   (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
        }

    return accum;
  }
};