#include <math.h>
#include <stdlib.h>

#include <iomanip>
#include <iostream>

#include "check.hpp"
#include "util.hpp"

class OneDimensionMonteCaloIntegration {
public:
  OneDimensionMonteCaloIntegration(const std::function<double(double)> &func,
                                   double l, double r, int sample_num)
      : func_{func}, l_{l}, r_{r}, is_finished_{false},
        sample_num_{sample_num} {
    CHECK(func_ != nullptr)
    CHECK(l_ < r_);
    CHECK(sample_num_ > 1)
  }
  void run() {
    double sample_value_sum{0.0};
    for (int i = 0; i < sample_num_; ++i) {
      double x = random_double(l_, r_);
      sample_value_sum += func_(x);
    }
    double expected_value = sample_value_sum / sample_num_;
    integration_result_ = expected_value * (r_ - l_);
    is_finished_ = true;
  }
  double get_result() {
    CHECK(is_finished_)
    return integration_result_;
  }

private:
  std::function<double(double)> func_;
  double r_;
  double l_;
  bool is_finished_;
  int sample_num_;
  double integration_result_;
};
int main() {
  int sample_num = 1e7;
  {
    auto func = [](double x) { return x * x; };
    double l = 0;
    double r = 2;
    OneDimensionMonteCaloIntegration x_2_integration{func, l, r, sample_num};
    x_2_integration.run();
    std::cout << "intergration x^2 from " << l << " to " << r
              << " ,result: " << x_2_integration.get_result() << "\n";
  }
  std::cout << "============================\n";
  {
    auto func = [](double x) { return pow(sin(x), 5.0); };
    double l = 0;
    double r = 2.0 * PI;
    OneDimensionMonteCaloIntegration x_2_integration{func, l, r, sample_num};
    x_2_integration.run();
    std::cout << "intergration sin(x)^5 from " << l << " to " << r
              << " ,result: " << x_2_integration.get_result() << "\n";
  }
  std::cout << "============================\n";
  {
    auto func = [](double x) { return log(sin(x)); };
    double l = 0;
    double r = PI;
    OneDimensionMonteCaloIntegration x_2_integration{func, l, r, sample_num};
    x_2_integration.run();
    std::cout << "intergration log(sin(x)) from " << l << " to " << r
              << " ,result: " << x_2_integration.get_result() << "\n";
  }
  return 0;
}