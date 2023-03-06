#include "../include/poller.hpp"

#include <unistd.h>

#include <algorithm>
#include <cstring>

#include "../include/util.hpp"

namespace falconlink {

Poller::Poller(uint32_t poll_size) : poll_size_(poll_size) {
  poll_fd_ = epoll_create1(0);
  errif(poll_fd_ == -1, "Poller create error");
  events_.resize(poll_size);
}

Poller::~Poller() {
  if (poll_fd_ != -1) {
    close(poll_fd_);
  }
}

std::vector<Channel *> Poller::poll(int timeout_ms) {
  int count = epoll_wait(poll_fd_, events_.data(), poll_size_, timeout_ms);
  errif(count == -1, "Poller wait error");

  std::vector<Channel *> active_channels;
  std::transform(events_.begin(), events_.begin() + count,
                 std::back_inserter(active_channels), [](const auto &event) {
                   Channel *ch = static_cast<Channel *>(event.data.ptr);
                   auto events = event.events;
                   if (events & EPOLLIN) {
                     ch->setReadyEvents(Channel::READ_EVENT);
                   }
                   if (events & EPOLLOUT) {
                     ch->setReadyEvents(Channel::WRITE_EVENT);
                   }
                   if (events & EPOLLET) {
                     ch->setReadyEvents(Channel::ET);
                   }
                   return ch;
                 });

  return active_channels;
}

void Poller::updateChannel(Channel *channel) {
  // contuct a event from channel
  int sockfd = channel->getSocket()->fd();
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.data.ptr = channel;

  if (channel->getListenEvents() & Channel::READ_EVENT) {
    ev.events |= EPOLLIN | EPOLLPRI;
  }
  if (channel->getListenEvents() & Channel::WRITE_EVENT) {
    ev.events |= EPOLLOUT;
  }
  if (channel->getListenEvents() & Channel::ET) {
    ev.events |= EPOLLET;
  }
  if (!channel->inPoller()) {
    errif(epoll_ctl(poll_fd_, EPOLL_CTL_ADD, sockfd, &ev) == -1,
          "epoll add error");
    channel->setInPoller(true);
  } else {
    errif(epoll_ctl(poll_fd_, EPOLL_CTL_MOD, sockfd, &ev) == -1,
          "epoll modify error");
  }
}

void Poller::deleteChannel(Channel *channel) {
  int sockfd = channel->getSocket()->fd();
  errif(epoll_ctl(poll_fd_, EPOLL_CTL_DEL, sockfd, nullptr) == -1,
        "epoll delete error");
  channel->setInPoller(false);
}

}  // namespace falconlink
