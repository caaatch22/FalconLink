#include "net/socket.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>  // for close(fd)

#include <algorithm>
#include <cassert>

#include "common/exception.hpp"

namespace falconlink {

Socket::Socket() {
  // TODO(catch22): supoort IPv6
  sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd_ == -1) {
    throw Exception(ExceptionType::SOCKET_ERROR, "Create socket error");
  }
}

Socket::Socket(Socket &&rhs) noexcept {
  sockfd_ = rhs.sockfd_;
  rhs.sockfd_ = -1;
}

Socket &Socket::operator=(Socket &&rhs) noexcept {
  if (sockfd_ != -1) {
    close(sockfd_);
  }
  std::swap(sockfd_, rhs.sockfd_);
  return *this;
}

Socket::~Socket() {
  if (sockfd_ != -1) {
    close(sockfd_);
    sockfd_ = -1;
  }
}

void Socket::bind(const InetAddr &addr) {
  if (::bind(sockfd_, reinterpret_cast<const sockaddr *>(addr.getAddr()),
             addr.getAddrLen()) == -1) {
    throw Exception(ExceptionType::SOCKET_ERROR, "Bind socket error");
  }
}

void Socket::bind(const char *ip, uint16_t port) { bind(InetAddr(ip, port)); }

void Socket::bind(const std::string &ip, uint16_t port) {
  bind(InetAddr(ip, port));
}

void Socket::listen() {
  assert(sockfd_ != -1 && "cannot Listen in an invalid fd");
  if (::listen(sockfd_, SOMAXCONN) < 0) {
    throw Exception(ExceptionType::SOCKET_ERROR, "Socket listen error");
  }
}

void Socket::connect(const InetAddr &addr) {
  if (isNonBlock()) { /** for client socket */
    while (true) {
      int res =
          ::connect(sockfd_, reinterpret_cast<const sockaddr *>(addr.getAddr()),
                    addr.getAddrLen());
      if (res == 0) {
        break;
      } else if (res == -1 && errno == EINPROGRESS) {
        continue; /* for simpicity, we made it block*/
      } else if (res == -1) {
        throw Exception(ExceptionType::SOCKET_ERROR, "Socket connect error");
      }
    }
  } else {
    int res =
        ::connect(sockfd_, reinterpret_cast<const sockaddr *>(addr.getAddr()),
                  addr.getAddrLen());
    if (res < 0) {
      throw Exception(ExceptionType::SOCKET_ERROR, "Socket connect error");
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
  assert(sockfd_ != -1 && "cannot accept with an invalid fd");
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
        throw Exception(ExceptionType::SOCKET_ERROR, "Socket accept error");
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
