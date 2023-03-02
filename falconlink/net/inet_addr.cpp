#include "../include/inet_addr.hpp"

#include <string.h>

namespace falconlink {

InetAddr::InetAddr() : addr_len_(sizeof(addr_)) {
  memset(&addr_, 0, sizeof(addr_));
}

InetAddr::InetAddr(const char* ip, uint16_t port) : addr_len_(sizeof(addr_)) {
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = inet_addr(ip);
  addr_.sin_port = htons(port);
  addr_len_ = sizeof(addr_);
}

InetAddr::InetAddr(const std::string& ip, uint16_t port)
    : InetAddr(ip.data(), port) {}

socklen_t InetAddr::getAddrLen() const { return addr_len_; }
const sockaddr_in* InetAddr::getAddr() const { return &addr_; }

struct sockaddr_in* InetAddr::yieldAddr() { return &addr_; }

socklen_t* InetAddr::yieldAddrLen() { return &addr_len_; }

std::string InetAddr::ip() const {
  char ip_address[1024];
  inet_ntop(AF_INET, &addr_.sin_addr, ip_address, INET_ADDRSTRLEN);
  return ip_address;
}

uint16_t InetAddr::port() const {
  return ntohs(addr_.sin_port);
}

}  // namespace falconlink
