#include <math.h>
#include <stdlib.h>

#include "check.hpp"
#include "print.hpp"
#include "util.hpp"
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

class UniformSamplingOnSphere {
public:
  UniformSamplingOnSphere() : rd_{}, gen_{rd_()}, d_{0.0} {}
  std::tuple<double, double, double> sample() {
    while (true) {
      double x = d_(gen_);
      double y = d_(gen_);
      double z = d_(gen_);

      if (x != 0 && y != 0 && z != 0) {
        double norm_2 = std::sqrt(x * x + y * y + z * z);
        return std::make_tuple(x / norm_2, y / norm_2, z / norm_2);
      }
    }
  }

private:
  std::random_device rd_;
  std::mt19937 gen_;
  std::normal_distribution<double> d_;
};

double mean(const std::vector<double> &vs) {
  double sum = std::accumulate(vs.begin(), vs.end(), 0.0);
  return sum / vs.size();
}
int main() {
  UniformSamplingOnSphere random_in_unit_sphere;
  std::vector<double> xs;
  std::vector<double> ys;
  std::vector<double> zs;
  int smaple_num = 100000;
  for (int i = 0; i < smaple_num; i++) {
    auto [x, y, z] = random_in_unit_sphere.sample();
    xs.push_back(x);
    ys.push_back(y);
    zs.push_back(z);
  }

  std::cout << "x-mean:" << mean(xs) << ", y-mean:" << mean(ys)
            << ",z-mean:" << mean(zs) << "\n";
  return 0;
}