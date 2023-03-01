#pragma once
#include <sys/epoll.h>
#include <vector>
#include "channel.hpp"

namespace falconlink {

constexpr int DEFAULT_EVENTS_LISTENED = 1024;

// TODO(catch22): figure out how to deal with exceptions
// TODO(catch22): consider whether to use singleton for Poller

class Channel;
/**
 * Encapsulation for epoll
 */
class Poller {
 public:
  explicit Poller(uint32_t poll_size = DEFAULT_EVENTS_LISTENED);
  ~Poller();

  void addFd(int fd, uint32_t op);
  void updateChannel(Channel*);
  auto poll(int timeout_ms = -1) -> std::vector<Channel*>;

 private:
  int poll_fd_;
  std::vector<epoll_event> events_;
  uint32_t pool_size_;
};

}  // namespace falconlink
