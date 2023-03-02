#pragma once

#include "inet_addr.hpp"

namespace falconlink {

/**
 * Wrapper class of raw c socket APIs
 * If any socket operation fail, use log to record instead of exceptions
*/
class Socket {

 public:
  Socket();
  explicit Socket(int sockfd) : sockfd_(sockfd) {}

  Socket(const Socket& rhs) = delete;
  Socket& operator=(const Socket& rhs) = delete;

  ~Socket();

  void bind(const InetAddr& addr);
  void listen();
  void setNonBlock();

  int accept(InetAddr& addr);

  int fd() const;

 private:
  int sockfd_{-1};
};

} // namespace falconlink
