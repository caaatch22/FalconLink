#pragma once

#include <functional>
#include <map>
#include <vector>

namespace falconlink {

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;


class Server {
 private:
  EventLoop *main_reactor_;
  Acceptor *acceptor_;
  std::map<int, Connection *> connections_;
  std::vector<EventLoop *> sub_reactors_;
  ThreadPool *thread_pool_;
  std::function<void(Connection *)> on_connect_callback_;
  std::function<void(Connection *)> on_message_callback_;
  std::function<void(Connection *)> new_connect_callback_;

 public:
  explicit Server(EventLoop *loop);
  ~Server();


  void newConnection(Socket *sock);
  void deleteConnection(Socket *sock);
  void onConnect(std::function<void(Connection *)> fn);
  void onMessage(std::function<void(Connection *)> fn);
  void newConnect(std::function<void(Connection *)> fn);
};

} // namespace falconlink
