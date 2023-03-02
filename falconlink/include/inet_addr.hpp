#pragma once
#include <arpa/inet.h>
#include <string>

namespace falconlink {

class InetAddr {
 public:
  InetAddr();

  explicit InetAddr(const char* ip, uint16_t port);

  InetAddr(const std::string& ip, uint16_t port);

  void setInetAddr(sockaddr_in addr, socklen_t addr_len);

  sockaddr_in getAddr() const;

  socklen_t getAddrLen() const;

  ~InetAddr() = default;

 private:
  struct sockaddr_in addr_;
  socklen_t addr_len_;
};
}
