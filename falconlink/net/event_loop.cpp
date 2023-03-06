#include "../include/event_loop.hpp"

#include <vector>

#include "../include/channel.hpp"
#include "../include/poller.hpp"

namespace falconlink {

EventLoop::EventLoop() { poller_ = new Poller(); }

EventLoop::~EventLoop() {
  Quit();
  delete poller_;
}

void EventLoop::loop() {
  while (!quit_) {
    std::vector<Channel *> chs = poller_->poll();
    for (const auto &ch : chs) {
      ch->handleEvent();
    }
  }
}

void EventLoop::updateChannel(Channel *ch) { poller_->updateChannel(ch); }
void EventLoop::deleteChannel(Channel *ch) { poller_->deleteChannel(ch); }
void EventLoop::Quit() { quit_ = true; }

}  // namespace falconlink
