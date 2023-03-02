#pragma once
#include "poller.hpp"
#include "channel.hpp"

namespace falconlink {

class Channel;
class Poller;

class EventLoop {
 public:
  EventLoop();
  ~EventLoop();

  void loop();
  void updateChannel(Channel *);

 private:
  Poller *poller_;
  bool quit_;
};

} // namespace falconlink

