#include "../include/poller.hpp"

#include <string.h>
#include <unistd.h>
#include <algorithm>

#include "../include/util.hpp"

namespace falconlink {

Poller::Poller(uint32_t pool_size) :pool_size_(pool_size) {
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

std::vector<Channel*> Poller::poll(int timeout_ms) {
  int count = epoll_wait(poll_fd_, events_.data(), pool_size_, timeout_ms);
  errif(count == -1, "Poller wait error");

  std::vector<Channel *> active_channels;
  std::transform(events_.begin(), events_.begin() + count,
                 std::back_inserter(active_channels), [](const auto &event) {
                   Channel *ch = static_cast<Channel *>(event.data.ptr);
                   ch->setRevents(event.events);
                   return ch;
                 });

  return active_channels;
}

void Poller::updateChannel(Channel *channel) {
  int fd = channel->fd();
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.data.ptr = channel;
  ev.events = channel->getEvents();
  if (!channel->inPoller()) {
    errif(epoll_ctl(poll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
    channel->setInPoller();
    // debug("Epoll: add Channel to epoll tree success, the Channel's fd is: ",
    // fd);
    } else{
        errif(epoll_ctl(poll_fd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
        // debug("Epoll: modify Channel in epoll tree success, the Channel's fd is: ", fd);
    }
}

} // namespace falconlink
