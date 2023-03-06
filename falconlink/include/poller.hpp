#pragma once
#include <sys/epoll.h>
#include <vector>
#include "channel.hpp"

namespace falconlink {

constexpr int DEFAULT_EVENTS_LISTENED = 1024;



// TODO(catch22): figure out how to deal with exceptions

class Channel;
/**
 * Encapsulation for epoll
 */
class Poller {
 public:
  explicit Poller(uint32_t poll_size = DEFAULT_EVENTS_LISTENED);
  ~Poller();

  void updateChannel(Channel*);
  void deleteChannel(Channel*);
  auto poll(int timeout_ms = -1) -> std::vector<Channel*>;

 private:
  /**returned fd when epoll_create1*/
  int poll_fd_;
  /**events get from epoll_wait*/
  std::vector<epoll_event> events_;

  uint32_t poll_size_;
};

}  // namespace falconlink
