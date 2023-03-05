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
  ~Channel();

  void handleEvent();
  void enableRead();
  void setReadCallback(std::function<void()> cb);

  int fd() const;
  uint32_t getEvents() const;
  uint32_t isReady() const;
  void setReady(uint32_t ready);
  bool inPoller() const;
  void setInPoller(bool in_poller);
  /*use edge trigger*/
  void useET();

 private:
  EventLoop *loop_;
  int fd_;
  /*to be listened, enum EPOLL_EVENTS*/
  uint32_t events_;
  uint32_t ready_;
  bool in_poller_;

  std::function<void()> read_callback_;
  std::function<void()> write_callback_;
};

}
