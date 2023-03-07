#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <thread>  // NOLINT
#include <mutex>   // NOLINT
#include <condition_variable>  // NOLINT
#include <future>      // NOLINT
#include <atomic>
#include <memory>
#include <utility>

#include "common/exception.hpp"
#include "common/macros.hpp"

namespace falconlink  {

/** The minimum number of threads to exist in the threadpool */
static constexpr int MIN_THREADS = 2;

class ThreadPool {
 public:
  explicit ThreadPool(int size = std::thread::hardware_concurrency() - 1);

  ~ThreadPool();

  NON_COPYABLE_AND_NON_MOVEABLE(ThreadPool);

  /** add new tasks to thread pool */
  template <class F, class... Args>
  decltype(auto) addTask(F&& f, Args&&... args);

  size_t size() const;

 private:
  std::vector<std::thread> workers_;

  std::queue<std::function<void()>> tasks_;

  /** lock the tasks */
  std::mutex tasks_mtx_;

  std::condition_variable cv_;

  std::atomic<bool> stop_{false};
};

template<typename F, typename ...Args>
decltype(auto) ThreadPool::addTask(F &&f, Args &&...args) {
  using return_type = typename std::invoke_result_t<F, Args...>;

  auto new_task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  // don't allow enqueueing after stopping the pool
  if (stop_) {
    throw Exception(ExceptionType::CONCURRENCY_ERROR,
                    "enqueue on stopped ThreadPool");
  }
  std::future<return_type> fut = new_task->get_future();
  {
    std::unique_lock<std::mutex> lock(tasks_mtx_);
    tasks_.emplace([task = std::move(new_task)]() { (*task)(); });
  }

  cv_.notify_one();
  return fut;
}

}  // namespace falconlink
