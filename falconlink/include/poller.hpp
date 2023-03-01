#pragma once
#include <sys/epoll.h>
#include <vector>

namespace falconlink {

constexpr int DEFAULT_EVENTS_LISTENED = 1024;


//TODO(catch22): figure out how to deal with exceptions
class Poller {
 public:
  explicit Poller(uint32_t poll_size = DEFAULT_EVENTS_LISTENED);
  ~Poller();

  void addFd(int fd, uint32_t op);
  std::vector<epoll_event> poll(int timeout_ms = -1);

 private:
  int poll_fd_;
  std::vector<epoll_event> events_;
  uint32_t pool_size_;
};
}
