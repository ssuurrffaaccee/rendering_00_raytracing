#pragma once
#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

// Usings
template <typename T>
using SPtr = std::shared_ptr<T>;
using std::make_shared;
using std::sqrt;

// Constants

const double INFINITY_ = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// Utility Functions

double degrees_to_radians(double degrees) { return degrees * PI / 180.0; }

double random_double() {
  // Returns a random real in [0,1).
  return rand() / (RAND_MAX + 1.0);
}

double random_double(double min, double max) {
  // Returns a random real in [min,max).
  return min + (max - min) * random_double();
}

int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max+1));
}
