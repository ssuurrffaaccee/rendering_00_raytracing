#include <math.h>
#include <stdlib.h>

#include <iomanip>
#include <iostream>

#include "check.hpp"
#include "util.hpp"

class TwoDimensionMonteCaloIntegration {
public:
  TwoDimensionMonteCaloIntegration(
      const std::function<double(double, double)> &func, double xl, double xr,
      double yl, double yr, int sample_num)
      : func_{func}, xr_{xr}, xl_{xl}, yr_{yr}, yl_{yl}, is_finished_{false},
        sample_num_{sample_num} {
    CHECK(func_ != nullptr)
    CHECK(xl_ < xr_);
    CHECK(yl_ < yr_);
    CHECK(sample_num_ > 1)
  }
  void run() {
    double sample_value_sum{0.0};
    for (int i = 0; i < sample_num_; ++i) {
      double x = random_double(xl_, xr_);
      double y = random_double(yl_, yr_);
      sample_value_sum += func_(x, y);
    }
    double expected_value = sample_value_sum / sample_num_;
    integration_result_ = expected_value * (xr_ - xl_) * (yr_ - yl_);
    is_finished_ = true;
  }
  double get_result() {
    CHECK(is_finished_)
    return integration_result_;
  }

private:
  std::function<double(double, double)> func_;
  double xr_;
  double xl_;
  double yr_;
  double yl_;
  bool is_finished_;
  int sample_num_;
  double integration_result_;
};
int main() {
  int sample_num = 1e7;
  {
    auto func = [](double x, double y) { return 1.0; };
    double xl = 0, xr = 1;
    double yl = 0, yr = 1;
    TwoDimensionMonteCaloIntegration constant_1_integration{
        func, xl, xr, yl, yr, sample_num};
    constant_1_integration.run();
    std::cout << "intergration constant 1.0 from " << xl << " to " << xr
              << " ,from " << yl << " to " << yr
              << " ,result: " << constant_1_integration.get_result() << "\n";
  }
  std::cout << "============================\n";
  {
    auto func = [](double x, double y) { return x * y; };
    double xl = 0, xr = 5;
    double yl = 0, yr = 6;
    TwoDimensionMonteCaloIntegration xy_integration{func, xl, xr,
                                                    yl,   yr, sample_num};
    xy_integration.run();
    std::cout << "intergration xy from " << xl << " to " << xr << " ,from "
              << yl << " to " << yr
              << " ,result: " << xy_integration.get_result() << "\n";
  }
  return 0;
}