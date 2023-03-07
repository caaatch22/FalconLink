#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "inet_addr.hpp"
#include "connection.hpp"
#include "common/macros.hpp"

namespace falconlink {

class EventLoop;
class InetAddr;

class Acceptor {
 public:
  explicit Acceptor(EventLoop *main_reactor, const InetAddr &server_addr,
                    std::vector<EventLoop *> sub_reactors);

  NON_COPYABLE_AND_NON_MOVEABLE(Acceptor);

  void basicAcceptBehavior(Connection *server_conn);

  ~Acceptor() = default;

  void setAcceptCallback(std::function<void(Connection *)> accept_callback);
  void setHandleCallback(std::function<void(Connection *)> handle_callback);

  std::function<void(Connection *)> getAcceptCallback() const;
  std::function<void(Connection *)> getHandleCallback() const;

 private:
  std::unique_ptr<Connection> acceptor_conn_;
  std::vector<EventLoop *> sub_reactors_;

  std::function<void(Connection *)> accept_callback_{};
  std::function<void(Connection *)> handle_callback_{};
};

}  // namespace falconlink
