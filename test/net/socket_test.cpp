#include "net/socket.hpp"

#include <fcntl.h>

#include <thread>  // NOLINT

#include "gtest/gtest.h"
#include "net/inet_addr.hpp"

namespace falconlink {

TEST(SocketTest, ConstuctorTest) {
  InetAddr local_host("127.0.0.1", 8090);
  Socket server_sock;

  server_sock.bind(local_host);
  ASSERT_NE(server_sock.fd(), -1);
  server_sock.listen();

  int orig_fd = server_sock.fd();
  Socket another_sock(1234);
  server_sock = std::move(another_sock);
  EXPECT_EQ(server_sock.fd(), 1234);
  EXPECT_EQ(another_sock.fd(), orig_fd);

  Socket second_sock = std::move(another_sock);
  EXPECT_EQ(second_sock.fd(), orig_fd);
  EXPECT_EQ(another_sock.fd(), -1);
}

}  // namespace falconlink
