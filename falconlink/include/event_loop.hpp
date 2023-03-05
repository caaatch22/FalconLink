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
  void addThread(std::function<void()>);

 private:
  /**owns the poller*/
  Poller *poller_;
  ThreadPool *thread_pool_;
  bool quit_;
};

} // namespace falconlink

