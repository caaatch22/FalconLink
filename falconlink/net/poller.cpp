#include "../include/poller.hpp"

#include <string.h>
#include <unistd.h>

#include "../include/util.hpp"

namespace falconlink {

Poller::Poller(unsigned pool_size) :pool_size_(pool_size) {
  poll_fd_ = epoll_create1(0);
  errif(poll_fd_ == -1, "Poller create error");
  events_.resize(pool_size);
}

Poller::~Poller() {
  if (poll_fd_ != -1) {
    close(poll_fd_);
    poll_fd_ = -1;
  }
}

void Poller::addFd(int fd, uint32_t op) {
  struct epoll_event event;
  memset(&event, 0, sizeof(event));
  event.data.fd = fd;
  event.events = op;
  errif(epoll_ctl(poll_fd_, EPOLL_CTL_ADD, fd, &event) == -1, "Poller add event error");
}

std::vector<epoll_event> Poller::poll(int timeout_ms) {
  int count = epoll_wait(poll_fd_, events_.data(), pool_size_, timeout_ms);
  errif(count == -1, "Poller wait error");
  return std::vector<epoll_event>(events_.begin(), events_.begin() + count);
}

}
