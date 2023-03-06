#include "../include/channel.hpp"

#include <unistd.h>

#include "../include/poller.hpp"

namespace falconlink {

Channel::Channel(EventLoop *loop, Socket *socket)
    : loop_(loop), socket_(socket) {}

Channel::~Channel() { loop_->deleteChannel(this); }

void Channel::enableRead() {
  listen_events_ |= READ_EVENT;
  loop_->updateChannel(this);
}

void Channel::enableWrite() {
  listen_events_ |= WRITE_EVENT;
  loop_->updateChannel(this);
}

void Channel::handleEvent() {
  if (ready_events_ & READ_EVENT) {
    read_callback_();
    }
  if (ready_events_ & WRITE_EVENT) {
    write_callback_();
  }
}

void Channel::useET() {
  listen_events_ |= ET;
  loop_->updateChannel(this);
}

Socket *Channel::getSocket() { return socket_; }

uint32_t Channel::getListenEvents() const { return listen_events_; }

uint32_t Channel::getReadyEvents() const { return ready_events_; }

bool Channel::inPoller() const { return in_poller_; }

void Channel::setInPoller(bool in_poller) { in_poller_ = in_poller; }

void Channel::setReadyEvents(uint32_t ev) {
  if (ev & READ_EVENT) {
    ready_events_ |= READ_EVENT;
  }
  if (ev & WRITE_EVENT) {
    ready_events_ |= WRITE_EVENT;
  }
  if (ev & ET) {
    ready_events_ |= ET;
  }
}

void Channel::setReadCallback(const std::function<void()> &callback) {
  read_callback_ = callback;
}
void Channel::setWriteCallback(const std::function<void()> &callback) {
  write_callback_ = callback;
}

}  // namespace falconlink
