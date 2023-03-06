#include "../include/socket.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>  // for close(fd)

#include <algorithm>
#include <cassert>

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

void Socket::bind(const InetAddr &addr) {
  ::bind(sockfd_, reinterpret_cast<const sockaddr *>(addr.getAddr()),
         addr.getAddrLen());
}

void Socket::bind(const char *ip, uint16_t port) { bind(InetAddr(ip, port)); }

void Socket::bind(const std::string &ip, uint16_t port) {
  bind(InetAddr(ip, port));
}

void Socket::listen() {
  int res = ::listen(sockfd_, SOMAXCONN);
  if (res < 0) {
    // TODO(catch22): record in log
  }
}

void Socket::connect(const InetAddr &addr) {
  // for client socket

  if (fcntl(sockfd_, F_GETFL) & O_NONBLOCK) {
    while (true) {
      int res =
          ::connect(sockfd_, reinterpret_cast<const sockaddr *>(addr.getAddr()),
                    addr.getAddrLen());
      if (res == 0) {
        break;
      }
      if (res == -1 && (errno == EINPROGRESS)) {
        continue;
        /* for simpicity, we made it block*/
      }
      if (res == -1) {
        errif(true, "socket connect error");
      }
    }
  } else {
    int res =
        ::connect(sockfd_, reinterpret_cast<const sockaddr *>(addr.getAddr()),
                  addr.getAddrLen());
    if (res < 0) {
      // TODO(catch22) : record in log;
    }
  }
}

void Socket::connect(const char *ip, uint16_t port) {
  connect(InetAddr(ip, port));
}

void Socket::connect(const std::string &ip, uint16_t port) {
  connect(InetAddr(ip, port));
}

void Socket::setNonBlock() {
  fcntl(sockfd_, F_SETFL, fcntl(sockfd_, F_GETFL) | O_NONBLOCK);
}

bool Socket::isNonBlock() const {
  return (fcntl(sockfd_, F_GETFL) & O_NONBLOCK) != 0;
}

int Socket::accept(InetAddr &addr) {
  int connection_fd = -1;
  if (isNonBlock()) {
    while (true) {
      connection_fd =
          ::accept(sockfd_, reinterpret_cast<sockaddr *>(addr.yieldAddr()),
                   addr.yieldAddrLen());
      if (connection_fd == -1 &&
          ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
        continue;
      }
      if (connection_fd == -1) {
        errif(true, "socket accept error");
      } else {
        break;
      }
    }
  } else {
    connection_fd =
        ::accept(sockfd_, reinterpret_cast<sockaddr *>(addr.yieldAddr()),
                 addr.yieldAddrLen());
    if (connection_fd < 0) {
      // TODO(catch22): record in log
    }
  }

  return connection_fd;
}

int Socket::fd() const { return sockfd_; }

}  // namespace falconlink
