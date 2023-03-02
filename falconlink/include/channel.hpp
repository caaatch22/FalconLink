#pragma once
#include <sys/epoll.h>
#include <functional>

#include "event_loop.hpp"
#include "poller.hpp"

namespace falconlink {

class EventLoop;
class Poller;

class Channel {
 public:
  Channel(EventLoop *loop, int fd);
  ~Channel() = default;

  void handleEvent();
  void enableReading();
  void setCallback(std::function<void()> cb);

  int fd() const;
  uint32_t getEvents() const;
  uint32_t getRevents() const;
  bool inPoller() const;
  void setInPoller();

  // void setEvents(uint32_t);
  void setRevents(uint32_t ev);
 private:
  EventLoop *loop_;
  int fd_;
  /*to be listened, enum EPOLL_EVENTS*/
  uint32_t events_;
  uint32_t revents_;
  bool in_poller_;

  std::function<void()> callback_;
};

}
