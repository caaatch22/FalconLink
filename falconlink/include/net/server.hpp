#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "net/acceptor.hpp"
#include "net/connection.hpp"
#include "net/inet_addr.hpp"
#include "net/event_loop.hpp"
#include "common/thread_pool.hpp"
#include "common/exception.hpp"

namespace falconlink {

/**
 * The class for setting up a web server using the FalconLink framework
 * Users should provide the callback functions in OnAccept() and OnHandle()
 * The rest is already taken care of and in most cases users don't need to touch
 * upon
 *
 * onAccept(): Given the acceptor connection, when the Poller tells us there is
 * new incoming client connection basic step of socket accept and build
 * connection and add into the Poller are already taken care of in the
 * Acceptor::basicAcceptBehavior. This OnAccept() functionality is appended to
 * that basicAcceptBehavior and called after that base, to support any
 * custom business logic upon receiving new client connection
 *
 * onHandle(): Users should implement provide a function
 * to achieve the expected behavior
 */
class Server {
 public:
  Server(InetAddr server_addr,
                        int concurrency = static_cast<int>(std::thread::hardware_concurrency()) -
                                          1)
      : thread_pool_(std::make_unique<ThreadPool>(concurrency)),
        main_reactor_(std::make_unique<EventLoop>()) {
    for (size_t i = 0; i < thread_pool_->size(); i++) {
      sub_reactors_.push_back(std::make_unique<EventLoop>());
    }
    for (auto &reactor : sub_reactors_) {
      thread_pool_->addTask([raw_reactor = reactor.get()] { raw_reactor->loop(); });
    }
    std::vector<EventLoop *> raw_reactors;
    raw_reactors.reserve(sub_reactors_.size());
    std::transform(sub_reactors_.begin(), sub_reactors_.end(),
                   std::back_inserter(raw_reactors),
                   [](auto &uni_ptr) { return uni_ptr.get(); });
    acceptor_ = std::make_unique<Acceptor>(main_reactor_.get(), server_addr,
                                           raw_reactors);
  }

  virtual ~Server() = default;

  /**
   * not edge trigger
   * @return return server itself for chain call 
  */
  Server& onAccept(std::function<void(Connection *)> on_accept) {
    acceptor_->setAcceptCallback(std::move(on_accept));
    return *this;
  }

  /* Edge trigger! Read all bytes please */
  Server &onHandle(std::function<void(Connection *)> on_handle) {
    acceptor_->setHandleCallback(std::move(on_handle));
    handle_set_ = true;
    return *this;
  }

  void start() {
    if (!handle_set_) {
      throw Exception(ExceptionType::CALLBACK_NOT_SET,
                      "Set handle callback before server starts");
    }
    main_reactor_->loop();
  }

 private:
  bool handle_set_{false};
  std::unique_ptr<Acceptor> acceptor_;
  std::vector<std::unique_ptr<EventLoop>> sub_reactors_;
  std::unique_ptr<ThreadPool> thread_pool_;
  std::unique_ptr<EventLoop> main_reactor_;
};
}  // namespace falconlink
