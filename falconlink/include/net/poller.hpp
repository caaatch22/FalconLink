#pragma once
#include <sys/epoll.h>

#include <memory>
#include <vector>

#include "common/macros.hpp"

namespace falconlink {

constexpr int DEFAULT_EVENTS_LISTENED = 1024;

static constexpr unsigned POLL_ADD = EPOLL_CTL_ADD;
static constexpr unsigned POLL_READ = EPOLLIN;
static constexpr unsigned POLL_ET = EPOLLET;

class Connection;

/**
 * Encapsulation for epoll
 */
class Poller {
 public:
  explicit Poller(uint32_t poll_size = DEFAULT_EVENTS_LISTENED);

  ~Poller();

  NON_COPYABLE(Poller);

  void addConnection(Connection*);

  std::vector<Connection *> poll(int timeout_ms = -1);

  uint32_t getPollSize() const;

 private:
  /**returned fd when epoll_create1*/
  int poll_fd_;
  /**events get from epoll_wait*/
  std::vector<epoll_event> events_;

  uint32_t poll_size_;
};

}  // namespace falconlink
