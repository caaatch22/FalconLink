#pragma once
#include <functional>
#include "buffer.hpp"

namespace falconlink {

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection {
 public:
  Connection(EventLoop *loop, Socket *sock);
  ~Connection();
    
  void echo(int sockfd);
  void setDeleteConnectionCallback(std::function<void(Socket*)>);

 private:
  EventLoop *loop_;
  Socket *sock_;
  Channel *channel_;
  std::function<void(Socket*)> deleteConnectionCallback;
  Buffer read_buf_;

};

} // namespace falconlink

