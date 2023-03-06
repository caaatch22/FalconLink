#pragma once
#include <sys/epoll.h>
#include <functional>

#include "event_loop.hpp"
#include "poller.hpp"
#include "socket.hpp"

namespace falconlink {

class EventLoop;
class Poller;
class Socket;

class Channel {
 public:
  explicit Channel(EventLoop *loop, Socket* socket);

  // disallow copy and move
  Channel(const Channel &rhs) = delete;
  Channel &operator=(const Channel &rhs) = delete;
  Channel(Channel &&rhs) = delete;
  Channel &operator=(Channel &&rhs) = delete;

  ~Channel();

  void handleEvent();
  void enableRead();
  void enableWrite();

  Socket *getSocket();

  uint32_t getListenEvents() const;
  uint32_t getReadyEvents() const;
  bool inPoller() const;

  void setReadyEvents(uint32_t ev);
  void setReadCallback(const std::function<void()> &callback);
  void setWriteCallback(const std::function<void()> &callback);
  void setInPoller(bool in_poller);

  /*use edge trigger*/
  void useET();

  static constexpr uint32_t READ_EVENT = 1;
  static constexpr uint32_t WRITE_EVENT = 2;
  static constexpr uint32_t ET = 4;

 private:
  EventLoop *loop_;

  Socket *socket_;
  /*events, enum EPOLL_EVENTS*/
  uint32_t listen_events_{0};
  uint32_t ready_events_{0};

  bool in_poller_{false};

  std::function<void()> read_callback_;
  std::function<void()> write_callback_;
};

}
