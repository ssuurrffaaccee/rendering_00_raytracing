#pragma once
#include <vector>

#include "bvh_node.hpp"
#include "hittable.hpp"
#include "material.hpp"
#include "triangle.hpp"
SPtr<Hittable> build_cow_mesh() {
#include "cow.inl"
  auto red = make_lambertian_material(Color{.65, .05, .05});
  SPtr<HittableList> mesh = std::make_shared<HittableList>();
  for (int i = 0; i < triangle_num; i++) {
    auto a_vertex_data_offset = vertex_index[i] * 3;
    auto a =
        Vec3{vertex[a_vertex_data_offset], vertex[a_vertex_data_offset + 1],
             vertex[a_vertex_data_offset + 2]};
    auto b_vertex_data_offset = vertex_index[i + 1] * 3;
    auto b =
        Vec3{vertex[b_vertex_data_offset], vertex[b_vertex_data_offset + 1],
             vertex[b_vertex_data_offset + 2]};
    auto c_vertex_data_offset = vertex_index[i + 2] * 3;
    auto c =
        Vec3{vertex[c_vertex_data_offset], vertex[c_vertex_data_offset + 1],
             vertex[c_vertex_data_offset + 2]};
    double scale = 25;
    mesh->add(make_triangle(a*scale, b*scale, c*scale, red));
  }
  return make_bvh_node(*mesh);
}