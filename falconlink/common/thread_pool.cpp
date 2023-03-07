#include "common/thread_pool.hpp"

#include <algorithm>

namespace falconlink {

ThreadPool::ThreadPool(int size) {
  size = std::max(size, MIN_THREADS);
  for (int i = 0; i < size; ++i) {
    workers_.emplace_back(std::thread([this]() {
      while (true) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(tasks_mtx_);
          cv_.wait(lock, [this]() { return stop_ || !tasks_.empty(); });
          if (stop_ && tasks_.empty()) {
            return;
          }
          task = std::move(tasks_.front());
          tasks_.pop();
        }
        task();
      }
    }));
  }
}

ThreadPool::~ThreadPool() {
  stop_ = true;
  cv_.notify_all();
  for (auto &worker : workers_) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

size_t ThreadPool::size() const { return workers_.size(); }

}  // namespace falconlink
