#pragma once
#include <functional>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
class BlockQueue {
public:
  void push(const std::function<void()> &task) {
    std::lock_guard lock{mutex_};
    elements_.push(task);
  }
  bool pop(std::function<void()> &new_task) {
    std::lock_guard lock{mutex_};
    if (elements_.empty()) {
      return false;
    }
    new_task = elements_.front();
    elements_.pop();
    return true;
  }
  bool empty() {
    std::lock_guard{mutex_};
    return elements_.empty();
  }

private:
  std::mutex mutex_;
  std::queue<std::function<void()>> elements_;
};
class ThreadManager {
public:
  ThreadManager(int num) {
    for (int i = 0; i < num; i++) {
      threads_.push_back(std::thread([this]() {
        std::function<void()> new_task;
        while (!stop_.load() || !this->task_queue_.empty()) {
          if (this->task_queue_.pop(new_task)) {
            new_task();
          }
        }
      }));
    }
  }
  ~ThreadManager() { wait(); }
  void wait() {
    for (auto &t : threads_) {
      if (t.joinable()) {
        t.join();
      }
    }
  }
  void stop_when_finish_all() {
    task_queue_.push([this]() { this->stop_.store(true); });
  }
  void run(const std::function<void()> &task) { task_queue_.push(task); }

private:
  BlockQueue task_queue_;
  std::atomic<bool> stop_{false};
  std::vector<std::thread> threads_;
};