#pragma once
#include "util.hpp"
class Interval {
  public:
    double min, max;

    Interval() : min(+INFINITY_), max(-INFINITY_) {} // Default Interval is empty

    Interval(double _min, double _max) : min(_min), max(_max) {}

    double size() const {
        return max - min;
    }

    Interval expand(double delta) const {
        auto padding = delta/2;
        return Interval(min - padding, max + padding);
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    static const Interval empty, universe;
};

const Interval Interval::empty    = Interval(+INFINITY_, -INFINITY_);
const Interval Interval::universe = Interval(-INFINITY_, +INFINITY_);