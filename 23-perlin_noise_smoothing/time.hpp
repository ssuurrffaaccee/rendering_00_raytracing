#pragma once
#include <chrono>
#include <iostream>
#include <string>
#define LOG(x) std::cout
#define TIMING

#ifdef TIMING
#define TIME_BEGIN(mark) \
  auto timing_##mark##_begin = std::chrono::high_resolution_clock::now();

#define TIME_END(mark)                                                  \
  auto timing_##mark##_end = std::chrono::high_resolution_clock::now(); \
  long timing_##mark##_duration =                                       \
      (long)std::chrono::duration_cast<std::chrono::microseconds>(      \
          timing_##mark##_end - timing_##mark##_begin)                  \
          .count();                                                     \
  LOG(INFO) << #mark << "_time_cost@" << timing_##mark##_duration/1e6 << '\n';
#else
#define TIME_BEGIN(mark)
#define TIME_END(mark)
#endif