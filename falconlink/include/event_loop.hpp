#pragma once
#include "channel.hpp"
#include "poller.hpp"
#include "thread_pool.hpp"

namespace falconlink {

class Channel;
class Poller;
class ThreadPool;

class EventLoop {
 public:
  EventLoop();
  ~EventLoop();

  void loop();
  void updateChannel(Channel *);
  void deleteChannel(Channel *);
  void Quit();

 private:
  /**owns the poller*/
  Poller *poller_{nullptr};
  bool quit_{false};
};

} // namespace falconlink

