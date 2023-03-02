#pragma once
#include <functional>

namespace falconlink {

class EventLoop;
class Socket;
class InetAddr;
class Channel;

class Acceptor {
 public:
  Acceptor(EventLoop *loop);
  ~Acceptor();
  void acceptConnection();
  void setNewConnectionCallback(std::function<void(Socket*)>);

 private:
  EventLoop *loop_;
  Socket *sock_;
  Channel *accept_channel_;
  std::function<void(Socket *)> newConnectionCallback;
};

} // namespace falconlink
