#pragma once
#include <arpa/inet.h>
#include <string>

namespace falconlink {

class InetAddr {
 public:
  InetAddr();

  explicit InetAddr(const char* ip, uint16_t port);

  InetAddr(const std::string& ip, uint16_t port);

  ~InetAddr() = default;

  struct sockaddr_in addr_;
  socklen_t addr_len_;
 private:

};
}
