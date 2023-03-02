#include "../include/acceptor.hpp"
#include "../include/socket.hpp"
#include "../include/inet_addr.hpp"
#include "../include/channel.hpp"

namespace falconlink {

Acceptor::Acceptor(EventLoop *loop) : loop_(loop) {
    sock_ = new Socket();
    addr_ = new InetAddr("127.0.0.1", 8888);
    sock_->bind(addr_);
    sock_->listen(); 
    sock_->setNonBlock();
    accept_channel_ = new Channel(loop, sock_->fd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    accept_channel_->setCallback(cb);
    accept_channel_->enableReading();
}

Acceptor::~Acceptor(){
    delete sock_;
    delete addr_;
    delete accept_channel_;
}

void Acceptor::acceptConnection(){
    newConnectionCallback(sock_);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> cb){
    newConnectionCallback = cb;
}

} // namespace falconlink
