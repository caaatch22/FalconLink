#pragma once

#include "inet_addr.hpp"

namespace falconlink {

/**
 * Wrap class of raw c socket APIs
 * If any socket operation fail, use log to record instead of exceptions
*/
class Socket {

 public:
  Socket();
  explicit Socket(int sockfd) : sockfd_(sockfd) {}
  ~Socket();

  void bind(InetAddr*);
  void listen();
  void setNonBlock();

  int accept(InetAddr*);

  int fd() const;

 private:
  int sockfd_{-1};
};

} // namespace falconlink
