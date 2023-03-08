#include "common/thread_pool.hpp"

#include <atomic>

#include "gtest/gtest.h"

namespace falconlink {

TEST(ThreadPoolTest, SimpleTest) {
  constexpr int thread_pool_size = 8;
  ThreadPool pool(thread_pool_size);
  ASSERT_EQ(pool.size(), thread_pool_size);

  std::atomic<int> var = 0;
  {
    ThreadPool local_pool(thread_pool_size);
    for (int i = 0; i < 3 * thread_pool_size; i++) {
      local_pool.addTask([&]() { var++; });
    }
    // here thread_pool's dtor should finish all the tasks
  }
    EXPECT_EQ(var, 3 * thread_pool_size);
}

}  // namespace falconlink
