#include "../include/channel.hpp"

#include "../include/poller.hpp"

namespace falconlink {

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), in_poller_(false) {}

void Channel::enableReading() {
  events_ = EPOLLIN | EPOLLET;
  loop_->updateChannel(this);
}

void Channel::handleEvent() { callback_(); }

void Channel::setCallback(std::function<void()> cb) { callback_ = cb; }

int Channel::fd() const { return fd_; }

uint32_t Channel::getEvents() const { return events_; }
uint32_t Channel::getRevents() const { return revents_; }

bool Channel::inPoller() const { return in_poller_; }

void Channel::setInPoller() { in_poller_ = true; }

// void Channel::setEvents(uint32_t _ev){
//     events = _ev;
// }

void Channel::setRevents(uint32_t ev) { revents_ = ev; }
}  // namespace falconlink
