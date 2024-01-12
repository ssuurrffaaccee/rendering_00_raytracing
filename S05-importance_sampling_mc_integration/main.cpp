#include <math.h>
#include <stdlib.h>

#include <iomanip>
#include <iostream>

#include "check.hpp"
#include "print.hpp"
#include "util.hpp"
#include <vector>
class PDF {
public:
  using Sample = double;
  virtual Sample sample() const = 0;
  virtual double pdf(Sample x) const = 0;
};
class ConstantPDF : public PDF {
public:
  ConstantPDF(double l, double r) : l_{l}, r_{r}, interval_length_{r - l} {
    CHECK(l_ < r_);
  }

  Sample sample() const override {
    return r_ + interval_length_ * random_double();
  }
  double pdf(Sample x) const override { 1.0 / interval_length_; }

private:
  double l_;
  double r_;
  double interval_length_;
};
class ImportanceSamplingOneDimensionMonteCaloIntegration {
public:
  ImportanceSamplingOneDimensionMonteCaloIntegration(
      const std::function<double(double)> &func, double l, double r,
      int sample_num, const std::shared_ptr<PDF> &pdf)
      : func_{func}, l_{l}, r_{r}, is_finished_{false},
        importance_sammpling_pdf_{pdf}, sample_num_{sample_num} {
    CHECK(func_ != nullptr)
    CHECK(importance_sammpling_pdf_ != nullptr)
    CHECK(l_ < r_);
    CHECK(sample_num_ > 1)
  }
  void run() {
    double sample_value_sum{0.0};
    for (int i = 0; i < sample_num_; ++i) {
      double x = importance_sammpling_pdf_->sample();
      sample_value_sum += func_(x) / importance_sammpling_pdf_->pdf(x);
    }
    // expected value a.k.a integration_result;
    integration_result_ = sample_value_sum / sample_num_;
    is_finished_ = true;
  }
  double get_result() {
    CHECK(is_finished_)
    return integration_result_;
  }

private:
  std::function<double(double)> func_;
  std::shared_ptr<PDF> importance_sammpling_pdf_;
  double r_;
  double l_;
  bool is_finished_;
  int sample_num_;
  double integration_result_;
};
int main() { return 0; }