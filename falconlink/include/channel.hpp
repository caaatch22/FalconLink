#pragma once
#include <sys/epoll.h>
#include "poller.hpp"

namespace falconlink {

class Poller;

class Channel {
 public:
  Channel(Poller *poller, int fd);
  ~Channel() = default;

  void enableReading();

  int fd() const;
  uint32_t getEvents() const;
  uint32_t getRevents() const;
  bool inPoller() const;
  void setInPoller();

  // void setEvents(uint32_t);
  void setRevents(uint32_t ev);
 private:
  Poller *poller_;
  int fd_;
  /*to be listened, enum EPOLL_EVENTS*/
  uint32_t events_;
  uint32_t revents_;
  bool in_poller_;
};
}
