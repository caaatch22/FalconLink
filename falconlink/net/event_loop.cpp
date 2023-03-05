#include "../include/event_loop.hpp"

#include <vector>

#include "../include/channel.hpp"
#include "../include/poller.hpp"

namespace falconlink {

EventLoop::EventLoop() : poller_(nullptr), quit_(false) {
  poller_ = new Poller();
  thread_pool_ = new ThreadPool();
}

EventLoop::~EventLoop() {
  delete poller_;
  delete thread_pool_;
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

void EventLoop::addThread(std::function<void()> func) { thread_pool_->add(func); }

}  // namespace falconlink
