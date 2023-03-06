#pragma once

#include <string>

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
  // TODO(catch22): mat allow move in the future
  Socket(Socket&& rhs) = delete;
  Socket& operator=(Socket&& rhs) = delete;

  ~Socket();

  void bind(const InetAddr& addr);
  void bind(const char* ip, uint16_t port);
  void bind(const std::string& ip, uint16_t port);

  void listen();
  
  void connect(const InetAddr& addr);
  void connect(const char* ip, uint16_t port);
  void connect(const std::string& ip, uint16_t port);

  void setNonBlock();
  bool isNonBlock() const;

  /**
   * @param[out] addr accepted peer(client) ip address 
  */
  int accept(InetAddr& addr);

  int fd() const;

 private:
  int sockfd_{-1};
};

} // namespace falconlink
