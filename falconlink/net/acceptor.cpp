#include "../include/acceptor.hpp"

#include "../include/channel.hpp"
#include "../include/inet_addr.hpp"
#include "../include/socket.hpp"

namespace falconlink {

Acceptor::Acceptor(EventLoop *loop) : loop_(loop) {
  sock_ = new Socket();
  sock_->bind("127.0.0.1", 8888);
  sock_->listen();
  accept_channel_ = new Channel(loop_, sock_);
  std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
  accept_channel_->setReadCallback(cb);
  accept_channel_->enableRead();
}

Acceptor::~Acceptor() {
  delete sock_;
  delete accept_channel_;
}

void Acceptor::acceptConnection() {
  InetAddr clnt_addr;
  Socket *conn_fd = new Socket(sock_->accept(clnt_addr));
  printf("new client fd %d, IP: %s, Port: %d\n", conn_fd->fd(),
         clnt_addr.ip().data(), clnt_addr.port());
  conn_fd->setNonBlock();
  if(new_connection_callback_) {
    new_connection_callback_(conn_fd);
  }
}

void Acceptor::setNewConnectionCallback(const std::function<void(Socket *)>& callback) {
  new_connection_callback_ = callback;
}

}  // namespace falconlink
