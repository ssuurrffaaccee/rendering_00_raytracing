#include <math.h>
#include <stdlib.h>

#include "check.hpp"
#include "print.hpp"
#include "util.hpp"
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>
std::tuple<double, double, double> random_in_unit_sphere() {
  while (true) {
    double x = random_double(-1, 1);
    double y = random_double(-1, 1);
    double z = random_double(-1, 1);
    if (x * x + y * y + z * z < 1.0f) {
      return std::make_tuple(x, y, z);
    }
  }
}
double mean(const std::vector<double> &vs) {
  double sum = std::accumulate(vs.begin(), vs.end(), 0.0);
  return sum / vs.size();
}
int main() {
  std::vector<double> xs;
  std::vector<double> ys;
  std::vector<double> zs;
  int smaple_num = 100000;
  for (int i = 0; i < smaple_num; i++) {
    auto [x, y, z] = random_in_unit_sphere();
    xs.push_back(x);
    ys.push_back(y);
    zs.push_back(z);
  }

  std::cout << "x-mean:" << mean(xs) << ", y-mean:" << mean(ys)
            << ",z-mean:" << mean(zs) << "\n";
  return 0;
}