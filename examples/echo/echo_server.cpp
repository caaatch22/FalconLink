#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include <unordered_map>

#include "../../falconlink/include/util.hpp"
#include "../../falconlink/include/poller.hpp"
#include "../../falconlink/include/inet_addr.hpp"
#include "../../falconlink/include/socket.hpp"
#include "../../falconlink/include/event_loop.hpp"
#include "../../falconlink/include/channel.hpp"
#include "../../falconlink/include/acceptor.hpp"
#include "../../falconlink/include/connection.hpp"

#define READ_BUFFER 1024

using falconlink::Acceptor;
using falconlink::Connection;
using falconlink::EventLoop;
using falconlink::Socket;

class Server {
 public:
  explicit Server(EventLoop *loop) :loop_(loop) {
    acceptor_ = new Acceptor(loop);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor_->setNewConnectionCallback(cb);
  }

  ~Server() { delete acceptor_; }

  void newConnection(Socket *sock) {
    Connection *conn = new Connection(loop_, sock);
    std::function<void(Socket*)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    conn->setDeleteConnectionCallback(cb);
    connections[sock->fd()] = conn;
  }

  void deleteConnection(Socket *sock) {
    Connection *conn = connections[sock->fd()];
    connections.erase(sock->fd());
    delete conn;
  }

 private:
  EventLoop *loop_;
  Acceptor *acceptor_;
  std::unordered_map<int, Connection *> connections;
};

int main() {
  EventLoop *loop = new EventLoop();
  Server *server = new Server(loop);
  loop->loop();

  delete loop;
  delete server;

  return 0;
}
