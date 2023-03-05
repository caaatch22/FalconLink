#include "../include/channel.hpp"

#include <unistd.h>

#include "../include/poller.hpp"

namespace falconlink {

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0), ready_(0), in_poller_(false), use_thread_pool_(true) {}

Channel::~Channel() {
  if(fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
}

void Channel::enableRead() {
  events_ |= (EPOLLIN | EPOLLPRI);
  loop_->updateChannel(this);
}

void Channel::handleEvent() {
  if (ready_ & (EPOLLIN | EPOLLPRI)) {
    if(use_thread_pool_) {
      loop_->addThread(read_callback_);
    } else {
      read_callback_();
    }
  } 
  if (ready_ & EPOLLOUT) {
    if (use_thread_pool_) {
      loop_->addThread(write_callback_);
    } else {
      write_callback_();
    }
  }
}

void Channel::useET() {
  events_ |= EPOLLET;
  loop_->updateChannel(this);
}

uint32_t Channel::isReady() const { return ready_; }

int Channel::fd() const { return fd_; }

uint32_t Channel::getEvents() const { return events_; }

bool Channel::inPoller() const { return in_poller_; }

void Channel::setInPoller(bool in_poller) { in_poller_ = in_poller; }

void Channel::setReadCallback(std::function<void()> cb) { read_callback_ = cb; }

void Channel::setUseThreadPool(bool use = true) { use_thread_pool_ = use; }

void Channel::setReady(uint32_t ready) { ready_ = ready; }

}  // namespace falconlink
