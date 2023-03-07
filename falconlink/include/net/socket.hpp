#pragma once

#include <string>

#include "inet_addr.hpp"
#include "common/macros.hpp"

namespace falconlink {

/**
 * Wrapper class of raw c socket APIs
*/
class Socket {
 public:
  /** used for server */
  Socket();

  explicit Socket(int sockfd) : sockfd_(sockfd) {}

  NON_COPYABLE(Socket);

  Socket(Socket &&rhs) noexcept;

  Socket& operator=(Socket&& rhs) noexcept;

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

}  // namespace falconlink
