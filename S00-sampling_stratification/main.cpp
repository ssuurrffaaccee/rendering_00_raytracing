#include <math.h>
#include <stdlib.h>

#include <iomanip>
#include <iostream>

#include "util.hpp"
bool is_inside_unit_circle(double x, double y) { return x * x + y * y < 1.0f; }
struct EstimatePiNaive {
  void kick() {
    total_sample_times_++;
    auto x = random_double(-1, 1);
    auto y = random_double(-1, 1);
    if (is_inside_unit_circle(x, y)) {
      inside_circle_sample_num_++;
    }
  }
  double get_pi() {
    return (4.0 * inside_circle_sample_num_) / total_sample_times_;
  }

 private:
  int inside_circle_sample_num_{0};
  int total_sample_times_{0};
};
struct EstimatePiStratification {
  void kick() {
    total_sample_times_++;
    auto x = 2 * ((i_ + random_double()) / sqrt_N_) - 1;
    auto y = 2 * ((j_ + random_double()) / sqrt_N_) - 1;
    if (is_inside_unit_circle(x, y)) {
      inside_circle_sample_num_++;
    }
    auto new_i = i_ + 1;
    i_ = new_i % sqrt_N_;
    j_ = (j_ + new_i / sqrt_N_) % sqrt_N_;
  }
  double get_pi() {
    return (4.0 * inside_circle_sample_num_) / total_sample_times_;
  }
  static int sqrt_N_;

 private:
  int inside_circle_sample_num_{0};
  int total_sample_times_{0};
  int i_{0};
  int j_{0};
};
int EstimatePiStratification::sqrt_N_{1000};
int main() {
  auto sqrt_N = EstimatePiStratification::sqrt_N_;
  auto sqrt_N_pow_2 = sqrt_N * sqrt_N;

  EstimatePiNaive estimate_pi_naive{};
  EstimatePiStratification estimate_pi_stratification{};
  for (int i = 0; i < sqrt_N_pow_2; i++) {
    estimate_pi_naive.kick();
    estimate_pi_stratification.kick();
  }
  std::cout << std::fixed << std::setprecision(12);
  std::cout << "Regular    Estimate of Pi = " << estimate_pi_naive.get_pi()
            << '\n'
            << "Stratified Estimate of Pi = "
            << estimate_pi_stratification.get_pi() << '\n';
  return 0;
}