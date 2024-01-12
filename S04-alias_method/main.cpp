#include <math.h>
#include <stdlib.h>

#include <iomanip>
#include <iostream>
#include <sstream>

#include "check.hpp"
#include "print.hpp"
#include "util.hpp"
#include <vector>
// https://www.keithschwarz.com/darts-dice-coins/
class AliasMethod {
public:
  void init(const std::vector<float> &weights) {
    prob_.resize(weights.size());
    alias_.resize(weights.size());
    std::vector<int64_t> small, large;
    std::vector<float> weights_(weights);
    double avg = 1 / static_cast<double>(weights_.size());
    for (size_t i = 0; i < weights_.size(); i++) {
      if (weights_[i] > avg) {
        large.push_back(i);
      } else {
        small.push_back(i);
      }
    }

    int64_t less, more;
    while (large.size() > 0 && small.size() > 0) {
      less = small.back();
      small.pop_back();
      more = large.back();
      large.pop_back();
      prob_[less] = weights_[less] * weights_.size();
      alias_[less] = more;
      weights_[more] = weights_[more] + weights_[less] - avg;
      if (weights_[more] > avg) {
        large.push_back(more);
      } else {
        small.push_back(more);
      }
    } // while (large.size() > 0 && small.size() > 0)
    while (small.size() > 0) {
      less = small.back();
      small.pop_back();
      prob_[less] = 1.0;
    }

    while (large.size() > 0) {
      more = large.back();
      large.pop_back();
      prob_[more] = 1.0;
    }
  }

  int64_t next() const {
    int64_t column = next_long(prob_.size());
    bool coinToss = random_double() < prob_[column];
    return coinToss ? column : alias_[column]; // if not me, must my paired, no
                                               // need to check probability;
  }

  size_t get_size() const { return prob_.size(); }

  std::string show_data() const {
    std::string result = "prob: {\n";
    for (auto &prob : prob_) {
      result += std::to_string(prob);
      result += "\n";
    }
    result += "}\n";

    result += "alias: {\n";
    for (auto &alias : alias_) {
      result += std::to_string(alias);
      result += "\n";
    }
    result += "}\n";

    return result;
  }

private:
  std::vector<float> prob_;    // curent slot's probability,
  std::vector<int64_t> alias_; // another paired variable
  int64_t next_long(int64_t n) const { return floor(n * random_double()); }
};
int main() {
  {
    std::vector<float> weights{0.3, 0.4, 0.3};
    AliasMethod alias_method;
    alias_method.init(weights);
    int sample_num = 100000;
    std::vector<int> sample_nums;
    sample_nums.resize(weights.size(), 0);
    for (int i = 0; i < sample_num; i++) {
      sample_nums[alias_method.next()]++;
    }
    std::vector<float> sample_freqency;
    sample_freqency.resize(weights.size());
    std::transform(sample_nums.begin(), sample_nums.end(),
                   sample_freqency.begin(),
                   [&](int &num) { return float(num) / sample_num; });
    std::cout << "weights   " << to_string(weights) << "\n";
    std::cout << "frequency " << to_string(sample_freqency) << "\n";
  }
  std::cout << "=============================\n";
  {
    std::vector<float> weights{0.1, 0.5, 0.4};
    AliasMethod alias_method;
    alias_method.init(weights);
    int sample_num = 100000;
    std::vector<int> sample_nums;
    sample_nums.resize(weights.size(), 0);
    for (int i = 0; i < sample_num; i++) {
      sample_nums[alias_method.next()]++;
    }
    std::vector<float> sample_freqency;
    sample_freqency.resize(weights.size());
    std::transform(sample_nums.begin(), sample_nums.end(),
                   sample_freqency.begin(),
                   [&](int &num) { return float(num) / sample_num; });
    std::cout << "weights   " << to_string(weights) << "\n";
    std::cout << "frequency " << to_string(sample_freqency) << "\n";
  }
}