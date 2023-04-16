#include "net/poller.hpp"

#include <unistd.h>

#include <algorithm>
#include <cstring>

#include "net/connection.hpp"
#include "common/logger.hpp"

namespace falconlink {

Poller::Poller(uint32_t poll_size) : poll_size_(poll_size) {
  poll_fd_ = epoll_create1(0);
  if (poll_fd_ == -1) {
    LOG_ERROR("Poller: epoll_create1() error");
    exit(EXIT_FAILURE);
  }
  events_.resize(poll_size);
}

Poller::~Poller() {
  if (poll_fd_ != -1) {
    close(poll_fd_);
    poll_fd_ = -1;
  }
}

void Poller::addConnection(Connection *conn) {
  assert(conn->fd() != -1 && "cannot AddConnection() with an invalid fd");
  struct epoll_event ev;
  memset(&ev, 0, sizeof ev);
  ev.data.ptr = conn;
  ev.events = conn->getEvents();
  int ret_val = epoll_ctl(poll_fd_, POLL_ADD, conn->fd(), &ev);
  if (ret_val == -1) {
    // TODO(catch22): deal with exceptions
  }
}

std::vector<Connection *> Poller::poll(int timeout_ms) {
  int count = epoll_wait(poll_fd_, events_.data(), poll_size_, timeout_ms);
  if (count == -1) {
    LOG_ERROR("Poller: Poll() error");
    exit(EXIT_FAILURE);
  }
  std::vector<Connection *> active_events;
  std::transform(events_.begin(), events_.begin() + count,
                 std::back_inserter(active_events), [](const auto &event) {
                   Connection *ready_conn =
                       static_cast<Connection *>(event.data.ptr);
                   ready_conn->setRevents(event.events);
                   return ready_conn;
                 });
  return active_events;
}

uint32_t Poller::getPollSize() const { return poll_size_; }

}  // namespace falconlink
