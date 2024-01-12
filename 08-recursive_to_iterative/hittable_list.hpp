#pragma once
#include <vector>

#include "hittable.hpp"
#include "util.hpp"
class HittableList : public Hittable {
 public:
  std::vector<SPtr<Hittable>> objects;

  HittableList() {}
  HittableList(SPtr<Hittable> object) { add(object); }

  void clear() { objects.clear(); }

  void add(SPtr<Hittable> object) { objects.push_back(object); }

  bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
    HitRecord temp_rec;
    auto hit_anything = false;
    auto closest_so_far = ray_t.max;
    for (const auto& object : objects) {
      if (object->hit(r, Interval{ray_t.min, closest_so_far}, temp_rec)) {
        hit_anything = true;
        closest_so_far = temp_rec.t_;
        rec = temp_rec;
      }
    }
    return hit_anything;
  }
};