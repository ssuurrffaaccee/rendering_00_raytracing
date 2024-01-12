#pragma once
#include <sstream>
#include <vector>

#include "hittable.hpp"
#include "util.hpp"
class HittableList : public Hittable {
 public:
  std::vector<SPtr<Hittable>> objects_;
  AABB bbox_;

  HittableList() {}
  HittableList(SPtr<Hittable> object) { add(object); }

  void clear() { objects_.clear(); }

  void add(SPtr<Hittable> object) {
    objects_.push_back(object);
    bbox_ = merge(bbox_, object->bounding_box());
  }

  bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
    HitRecord temp_rec;
    auto hit_anything = false;
    auto closest_so_far = ray_t.max;
    for (const auto& object : objects_) {
      if (object->hit(r, Interval{ray_t.min, closest_so_far}, temp_rec)) {
        hit_anything = true;
        closest_so_far = temp_rec.t_;
        rec = temp_rec;
      }
    }
    return hit_anything;
  }
  AABB bounding_box() const override { return bbox_; }
  std::string to_string() const override {
    std::stringstream ss;
    ss << "List(";
    if (objects_.size() == 0) {
      ss << ")";
    } else if (objects_.size() == 1) {
      ss << objects_[0]->to_string() << ")";
    } else {
      ss << objects_[0]->to_string();
      for (auto iter = objects_.begin() + 1; iter != objects_.end(); iter++) {
        ss << "," << (*iter)->to_string();
      }
      ss << ")";
    }
    return ss.str();
  }
};