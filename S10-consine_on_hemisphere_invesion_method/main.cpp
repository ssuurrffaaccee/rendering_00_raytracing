#include <math.h>
#include <stdlib.h>

#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#include "check.hpp"
#include "print.hpp"
#include "util.hpp"
// p(ω)=f(θ)
// having p(θ,ϕ)=sin(θ)*p(ω)
// so: p(θ)=1/(2*pi);
//     p(ϕ|θ) = p(θ,ϕ)/p(θ) = sin(θ)*p(ω)/(1/(2*pi)) = 2*pi*sin(θ)*p(ω) =
//     2*pi*sin(θ)*f(θ)
class ConsineSamplingOnHemiSphere {
 public:
  ConsineSamplingOnHemiSphere() : rd_{}, gen_{rd_()}, d_{0.0} {}
  std::tuple<double, double, double> sample() {
    auto r1 = random_double();
    auto r2 = random_double();
    auto phi = 2*PI*r1;
    auto x = cos(phi)*sqrt(r2);
    auto y = sin(phi)*sqrt(r2);
    auto z = sqrt(1-r2);
    return std::make_tuple(x, y, z);
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
  ConsineSamplingOnHemiSphere cosine_in_unit_hemi_sphere;
  std::vector<double> xs;
  std::vector<double> ys;
  std::vector<double> zs;
  int smaple_num = 100000;
  for (int i = 0; i < smaple_num; i++) {
    auto [x, y, z] = cosine_in_unit_hemi_sphere.sample();
    xs.push_back(x);
    ys.push_back(y);
    zs.push_back(z);
  }

  std::cout << "x-mean:" << mean(xs) << ", y-mean:" << mean(ys)
            << ",z-mean:" << mean(zs) << "\n";
  return 0;
}