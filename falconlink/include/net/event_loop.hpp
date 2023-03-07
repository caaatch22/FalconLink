#pragma once

#include <mutex>  // NOLINT
#include <unordered_map>
#include <memory>

#include "net/poller.hpp"

namespace falconlink {

class Connection;

class EventLoop {
 public:
  EventLoop() = default;

  ~EventLoop();

  void addAcceptor(Connection *acceptor_conn);

  void addConnection(std::unique_ptr<Connection> new_conn);

  bool deleteConnection(int fd);

  /**Infinite loop to catch events*/
  void loop();

  void quit();

 private:
  Poller poller_{};

  std::mutex mtx_;
  std::unordered_map<int, std::unique_ptr<Connection>> connections_;

  bool quit_{false};
};

}  // namespace falconlink

