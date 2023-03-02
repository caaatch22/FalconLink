#include "../include/acceptor.hpp"
#include "../include/socket.hpp"
#include "../include/inet_addr.hpp"
#include "../include/channel.hpp"

namespace falconlink {

Acceptor::Acceptor(EventLoop *loop) : loop_(loop) {
  sock_ = new Socket();
  InetAddr *addr = new InetAddr("127.0.0.1", 8888);
  sock_->bind(addr);
  sock_->listen();
  sock_->setNonBlock();
  accept_channel_ = new Channel(loop, sock_->fd());
  std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
  accept_channel_->setCallback(cb);
  accept_channel_->enableReading();
  delete addr;
}

Acceptor::~Acceptor(){
  delete sock_;
  delete accept_channel_;
}

void Acceptor::acceptConnection() {
  InetAddr *clnt_addr = new InetAddr();
  Socket *clnt_sock = new Socket(sock_->accept(clnt_addr));
  printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->fd(),
         inet_ntoa(clnt_addr->getAddr().sin_addr),
         ntohs(clnt_addr->getAddr().sin_port));
  clnt_sock->setNonBlock();
  newConnectionCallback(clnt_sock);
  delete clnt_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> cb) {
  newConnectionCallback = cb;
}

} // namespace falconlink
