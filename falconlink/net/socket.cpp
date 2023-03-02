#include "../include/socket.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
#include <string.h>

#include "../include/util.hpp"

namespace falconlink {

Socket::Socket() {
  sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd_ == -1) {
    // TODO(catch22): record in log
  }
}

Socket::~Socket() {
  if (sockfd_ != -1) {
    close(sockfd_);
    sockfd_ = -1;
  }
}

void Socket::bind(InetAddr *_addr){
  struct sockaddr_in addr = _addr->getAddr();
  socklen_t addr_len = _addr->getAddrLen();
  errif(::bind(sockfd_, (sockaddr *)&addr, addr_len) == -1, "socket bind error");
  _addr->setInetAddr(addr, addr_len);
}

void Socket::listen() {
  int res = ::listen(sockfd_, SOMAXCONN);
  if (res < 0) {
    // TODO(catch22): record in log
  }
}
void Socket::setNonBlock() {
  fcntl(sockfd_, F_SETFL, fcntl(sockfd_, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddr *_addr) {
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  bzero(&addr, sizeof(addr));
  int clnt_sockfd = ::accept(sockfd_, (sockaddr *)&addr, &addr_len);
  errif(clnt_sockfd == -1, "socket accept error");
  _addr->setInetAddr(addr, addr_len);
  return clnt_sockfd;
}

int Socket::fd() const { return sockfd_; }

}  // namespace falconlink