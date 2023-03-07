#include "net/event_loop.hpp"

#include <vector>

#include "net/connection.hpp"

namespace falconlink {

EventLoop::~EventLoop() { quit(); }

void EventLoop::loop() {
  while (!quit_) {
    std::vector<Connection *> ready_conns = poller_.poll();
    for (auto &conn : ready_conns) {
      conn->getCallback()();
    }
  }
}

void EventLoop::addAcceptor(Connection *acceptor_conn) {
  std::unique_lock<std::mutex> lock(mtx_);
  poller_.addConnection(acceptor_conn);
}

void EventLoop::addConnection(std::unique_ptr<Connection> new_conn) {
  std::unique_lock<std::mutex> lock(mtx_);
  poller_.addConnection(new_conn.get());
  int fd = new_conn->fd();
  connections_.insert({fd, std::move(new_conn)});
}

bool EventLoop::deleteConnection(int fd) {
  std::unique_lock<std::mutex> lock(mtx_);
  auto it = connections_.find(fd);
  if (it == connections_.end()) {
    return false;
  }
  connections_.erase(it);
  return true;
}

void EventLoop::quit() { quit_ = true; }

}  // namespace falconlink
