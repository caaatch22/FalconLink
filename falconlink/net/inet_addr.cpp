#include "../include/inet_addr.hpp"

#include <string.h>

namespace falconlink {

InetAddr::InetAddr() {
  memset(&addr_, 0, sizeof(addr_));
  addr_len_ = sizeof(addr_);
}

InetAddr::InetAddr(const char* ip, uint16_t port)
    : addr_len_(sizeof(addr_)) {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = inet_addr(ip);
  addr_.sin_port = htons(port);
  addr_len_ = sizeof(addr_);
}

InetAddr::InetAddr(const std::string& ip, uint16_t port)
    : InetAddr(ip.data(), port) {}

sockaddr_in InetAddr::getAddr() const { return addr_; }

socklen_t InetAddr::getAddrLen() const { return addr_len_; }


void InetAddr::setInetAddr(sockaddr_in addr, socklen_t addr_len){
  addr_ = addr;
  addr_len_ = addr_len;
}

}
