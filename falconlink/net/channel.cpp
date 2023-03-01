#include "../include/channel.hpp"
#include "../include/poller.hpp"

namespace falconlink {

Channel::Channel(Poller *poller, int fd)
    : poller_(poller), fd_(fd), events_(0), revents_(0), in_poller_(false) {}

void Channel::enableReading(){
    events_ = EPOLLIN | EPOLLET;
    poller_->updateChannel(this);
}

int Channel::fd() const { return fd_; }

uint32_t Channel::getEvents() const { return events_; }
uint32_t Channel::getRevents() const { return revents_; }

bool Channel::inPoller() const { return in_poller_; }

void Channel::setInPoller() { in_poller_ = true; }

// void Channel::setEvents(uint32_t _ev){
//     events = _ev;
// }

void Channel::setRevents(uint32_t ev) { revents_ = ev; }
}
