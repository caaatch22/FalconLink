#include "../include/socket.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>

#include "../include/poller.hpp"
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

void Socket::bind(InetAddr *addr) {
  int res = ::bind(sockfd_, (sockaddr *)&addr->addr_, addr->addr_len_);
  if (res < 0) {
    // TODO(catch22): record in log
  }
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

int Socket::accept(InetAddr *addr) {
  int client_sockfd = ::accept(sockfd_, (sockaddr *)&addr->addr_, &addr->addr_len_);
  if (client_sockfd < 0) {
    // TODO(catch22): record in log
  }
  return client_sockfd;
}

int Socket::fd() const { return sockfd_; }

}  // namespace falconlink