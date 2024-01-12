#pragma once
#include <algorithm>
#include <stack>

#include "check.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
SPtr<Hittable> make_bvh_node_internal_(std::vector<SPtr<Hittable>>& objects,
                                       int start, int end);
class BVHNode : public Hittable {
 public:
  BVHNode() {}
  bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
    if (!bbox_.hit(r, ray_t)) {
      return false;
    }
    bool hit_left = left_->hit(r, ray_t, rec);  // hit left
    // if hit_left == true, [rec.t_, ray_t.mx] has explored by left.hit;,so just
    // explore inervel [ray_t.min,rec.t_]
    bool hit_right =
        right_->hit(r, Interval(ray_t.min, hit_left ? rec.t_ : ray_t.max), rec);
    return hit_left || hit_right;
  }
  AABB bounding_box() const override { return bbox_; }
  friend SPtr<Hittable> make_bvh_node_internal_(
      std::vector<SPtr<Hittable>>& objects, int start, int end);
  std::string to_string() const override {
    std::stringstream ss;
    ss << "Tree(";
    ss << left_->to_string();
    ss << ",";
    ss << right_->to_string();
    ss << ")";
    return ss.str();
  }

 private:
  SPtr<Hittable> left_;   // BVHNode or other
  SPtr<Hittable> right_;  // BVHNode or other
  AABB bbox_;
};
bool box_compare(const SPtr<Hittable> a, const SPtr<Hittable> b,
                 int axis_index) {
  return a->bounding_box().axis(axis_index).min <
         b->bounding_box().axis(axis_index).min;
}

bool box_x_compare(const SPtr<Hittable> a, const SPtr<Hittable> b) {
  return box_compare(a, b, 0);
}

bool box_y_compare(const SPtr<Hittable> a, const SPtr<Hittable> b) {
  return box_compare(a, b, 1);
}

bool box_z_compare(const SPtr<Hittable> a, const SPtr<Hittable> b) {
  return box_compare(a, b, 2);
}
SPtr<Hittable> make_bvh_node(const HittableList& world) {
  auto objects = world.objects_;
  CHECK(objects.size() > 0);
  return make_bvh_node_internal_(objects, 0, objects.size());
}

SPtr<Hittable> make_bvh_node_internal_(std::vector<SPtr<Hittable>>& objects,
                                       int start, int end) {
  int axis = random_int(0, 2);
  auto comparator = (axis == 0)   ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;
  size_t object_span = end - start;
  BVHNode* bvh_node = new BVHNode{};
  if (object_span == 1) {
    bvh_node->left_ = objects[start];
    bvh_node->right_ = objects[start];
  } else if (object_span == 2) {
    if (comparator(objects[start], objects[start + 1])) {
      bvh_node->left_ = objects[start];
      bvh_node->right_ = objects[start + 1];
    } else {
      bvh_node->left_ = objects[start + 1];
      bvh_node->right_ = objects[start];
    }
  } else {
    std::sort(objects.begin() + start, objects.begin() + end, comparator);
    auto mid = start + object_span / 2;
    bvh_node->left_ = make_bvh_node_internal_(objects, start, mid);
    bvh_node->right_ = make_bvh_node_internal_(objects, mid, end);
  }
  bvh_node->bbox_ =
      merge(bvh_node->left_->bounding_box(), bvh_node->right_->bounding_box());
  return std::shared_ptr<Hittable>{(Hittable*)bvh_node};
}