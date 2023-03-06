#pragma once
#include <arpa/inet.h>
#include <string>

namespace falconlink {

class InetAddr {
 public:
  InetAddr();

  explicit InetAddr(const char* ip, uint16_t port);

  explicit InetAddr(const std::string& ip, uint16_t port);

  /**for reading resource*/
  socklen_t getAddrLen() const;
  const sockaddr_in* getAddr() const;

  /**cooperate with C APIs who uses ptr as param[out]*/
  struct sockaddr_in* yieldAddr();
  socklen_t* yieldAddrLen();

  std::string ip() const;

  uint16_t port() const;

  ~InetAddr() = default;

 private:
  struct sockaddr_in addr_ {};
  socklen_t addr_len_;
};
}
