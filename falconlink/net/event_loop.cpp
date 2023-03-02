#include "../include/event_loop.hpp"
#include "../include/poller.hpp"
#include "../include/channel.hpp"

#include <vector>

namespace falconlink {

EventLoop::EventLoop() : poller_(nullptr), quit_(false) {
  poller_ = new Poller();
}

EventLoop::~EventLoop() { delete poller_; }

void EventLoop::loop() {
  while(!quit_) {
    std::vector<Channel *> chs = poller_->poll();
    for (const auto& ch: chs) {
      ch->handleEvent();
    }
  }
}

void EventLoop::updateChannel(Channel *ch) { poller_->updateChannel(ch); }

}
