#pragma once
#include <functional>

namespace falconlink {

class EventLoop;
class Socket;
class InetAddr;
class Channel;

class Acceptor {
 public:
  explicit Acceptor(EventLoop *loop);

  // disallow copy and move
  Acceptor(const Acceptor &rhs) = delete;
  Acceptor &operator=(const Acceptor &rhs) = delete;
  Acceptor(Acceptor &&rhs) = delete;
  Acceptor &operator=(Acceptor &&rhs) = delete;
  
  ~Acceptor();

  void acceptConnection();

  void setNewConnectionCallback(const std::function<void(Socket*)>& callback);

 private:
  EventLoop *loop_;
  Socket *sock_;
  Channel *accept_channel_;
  std::function<void(Socket *)> new_connection_callback_;
};

} // namespace falconlink
