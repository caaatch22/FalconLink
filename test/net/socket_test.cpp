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
  CHECK(server_sock.fd() == 1234);
  CHECK(another_sock.fd() == orig_fd);

  Socket second_sock = std::move(another_sock);
  EXPECT_EQ(second_sock.fd(), orig_fd);
  EXPECT_EQ(another_sock.GetFd(), -1);

}

TEST(SocketTest, BlockTest) {
  InetAddr local_host("127.0.0.1", 8090);
  Socket sock;
  sock.bind(local_host);
  CHECK((sock.GetAttrs() & O_NONBLOCK) == 0);
  sock.SetNonBlocking();
  CHECK((sock.GetAttrs() & O_NONBLOCK) != 0);

  SECTION("interaction between server and client socket") {
    InetAddr client_address;
    std::thread client_thread = std::thread([&]() {
      Socket client_sock;
      client_sock.Connect(local_host);
    });
    client_thread.detach();
    EXPECT_NE(server_sock.accept(client_address), -1);
  }
}


}  // namespace falconlink
