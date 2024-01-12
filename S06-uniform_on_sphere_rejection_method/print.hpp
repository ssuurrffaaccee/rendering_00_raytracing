#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "check.hpp"
template <typename T>
std::string to_string(const T& v) {
  std::stringstream ss;
  ss << v;
  return ss.str();
}

template <typename T>
std::string to_string(const std::vector<T>& vs) {
  std::stringstream ss;
  ss << "array(";
  for (auto& v : vs) {
    ss << v << ", ";
  }
  ss << ")";
  return ss.str();
}

std::vector<int> int_range(int left, int right) {
  CHECK(right != left)
  std::vector<int> res;
  if (left < right) {
    res.reserve(right - left);
    for (int i = left; i < right; i++) {
      res.push_back(i);
    }
    return res;
  } else {
    res.reserve(left - right);
    for (int i = left; i > right; i--) {
      res.push_back(i);
    }
    return res;
  }
}

std::vector<int> int_range(int right) {
  if (right > 0) {
    return int_range(0, right);
  } else {
    return int_range(right, 0);
  }
}

#define PRINT(x) std::cout<<x;