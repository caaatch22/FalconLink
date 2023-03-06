#pragma once
#include <signal.h>
#include <functional>
#include <map>

namespace falconlink {

std::map<int, std::function<void()>> handlers_;
void signal_handler(int sig) {
  handlers_[sig]();
}

struct Signal {
  static void signal(int sig, const std::function<void()> &handler) {
    handlers_[sig] = handler;
    ::signal(sig, signal_handler);
  }
};

}
