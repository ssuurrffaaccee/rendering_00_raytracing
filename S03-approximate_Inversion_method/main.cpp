#include <math.h>
#include <stdlib.h>

#include <iomanip>
#include <iostream>
#include <sstream>

#include "check.hpp"
#include "util.hpp"
#include <vector>

struct Sample {
  double x_;
  double p_;
};
std::string to_string(const std::vector<Sample> &vs) {
  std::stringstream ss;
  ss << "array(";
  for (auto &v : vs) {
    ss << "Sample(" << v.x_ << "," << v.p_ << ")\n";
  }
  ss << ")";
  return ss.str();
}
class ApproximateInversionMethod {
public:
  ApproximateInversionMethod(const std::function<double(double)> &pdf_func,
                             double l, double r, int sample_num)
      : pdf_func_{pdf_func}, l_{l}, r_{r}, sample_num_{sample_num} {
    init();
  }
  double sample() {
    double p = random_double();
    return inversion_func_[int(p * sample_num_ - 1)];
  }
  double debug(double x) { return inversion_func_[int(x * sample_num_ - 1)]; }

private:
  void init() {
    CHECK(pdf_func_ != nullptr)
    CHECK(l_ < r_)
    std::vector<Sample> samples_;
    double dx = (r_ - l_) / sample_num_;
    samples_.resize(sample_num_);
    samples_[0] = Sample{l_, 0};
    for (int i = 1; i < sample_num_; i++) {
      double x = i * dx + l_;
      samples_[i] = Sample{x, pdf_func_(x) + samples_[i - 1].p_ * dx};
    }
    std::sort(samples_.begin(), samples_.end(),
              [](const Sample &s1, const Sample &s2) { return s1.x_ < s2.x_; });
    inversion_func_.resize(sample_num_);
    for (int i = 0; i < sample_num_; i++) {
      double p = double(i) / sample_num_;
      double lower_p{0};
      double lower_x{0};
      double upper_p{0};
      double upper_x{0};
      auto lower = std::lower_bound(
          samples_.begin(), samples_.end(), p,
          [](const Sample &s1, double value) { return s1.p_ < value; });
      if (lower == samples_.end()) {
        lower_p = 0;
        lower_x = l_;
      } else {
        lower_p = lower->p_;
        lower_x = lower->x_;
      }
      auto upper = std::upper_bound(
          samples_.begin(), samples_.end(), p,
          [](double value, const Sample &s1) { return value < s1.p_; });
      if (upper == samples_.end()) {
        upper_p = 1.0;
        upper_x = r_;
      } else {
        upper_p = upper->p_;
        upper_x = upper_x;
      }
      if (p == lower_p) {
        inversion_func_[i] = lower_x;
        continue;
      } else if (p == upper_x) {
        inversion_func_[i] = upper_x;
        continue;
      } else {
        if (lower_p == upper_p) {
          inversion_func_[i] = upper_p;
        } else {
          inversion_func_[i] = lower_x + (upper_x - lower_x) * (upper_p - p) /
                                             (upper_p - lower_p);
        }
      }
    }
  }
  double r_;
  double l_;
  std::vector<double> inversion_func_;
  int sample_num_;
  std::function<double(double)> pdf_func_;
};
int main() {
  {
    auto pdf_func = [](double x) { return x; };
    double l = 0;
    double r = 1;
    int sample_num = 1000;
    ApproximateInversionMethod approximate_inversion_method{pdf_func, l, r,
                                                            sample_num};
    for (int i = 0; i < 100; i++) {
      float x = float(i) / 100;
      auto value = approximate_inversion_method.debug(x);
      std::cout << x << " " << value << "\n";
    }
  }
  return 0;
}