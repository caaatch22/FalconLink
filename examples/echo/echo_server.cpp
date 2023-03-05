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
#include "../../falconlink/include/thread_pool.hpp"

#define READ_BUFFER 1024

using falconlink::Acceptor;
using falconlink::Connection;
using falconlink::EventLoop;
using falconlink::Socket;
using falconlink::ThreadPool;

class Server {
 public:
  explicit Server(EventLoop *loop) :main_reactor_(loop) {
    acceptor_ = new Acceptor(loop);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor_->setNewConnectionCallback(cb);

    auto size = std::thread::hardware_concurrency();
    thread_pool_ = new ThreadPool(size);
    for (unsigned int i = 0; i < size; ++i) {
      sub_reactors_.push_back(new EventLoop());
    }

    for(unsigned int i = 0; i < size; ++i){
      std::function<void()> sub_loop =
          std::bind(&EventLoop::loop, sub_reactors_[i]);
      thread_pool_->add(sub_loop);
    }
  }

  ~Server() {
    delete acceptor_;
    delete thread_pool_;
  }

  void newConnection(Socket *sock) {
    if (sock->fd() != -1) {
      int random = sock->fd() % static_cast<int>(sub_reactors_.size());
      Connection *conn = new Connection(sub_reactors_[random], sock);
      std::function<void(int)> cb =
          std::bind(&Server::deleteConnection, this, std::placeholders::_1);
      conn->setDeleteConnectionCallback(cb);
      connections[sock->fd()] = conn;
    }
  }

  void deleteConnection(int sockfd) {
    if (sockfd != -1) {
      auto it = connections.find(sockfd);
      if (it != connections.end()) {
        Connection *conn = connections[sockfd];
        connections.erase(sockfd);
        delete conn;
      }
    }
  }

 private:
  EventLoop *main_reactor_;
  Acceptor *acceptor_;
  std::unordered_map<int, Connection *> connections;
  std::vector<EventLoop *> sub_reactors_;
  ThreadPool *thread_pool_;
};

int main() {
  EventLoop *loop = new EventLoop();
  Server *server = new Server(loop);
  loop->loop();

  delete loop;
  delete server;

  return 0;
}
