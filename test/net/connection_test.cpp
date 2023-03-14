#include "net/connection.hpp"

#include <unistd.h>

#include <cstring>
#include <memory>
#include <string>
#include <thread>  // NOLINT

#include "gtest/gtest.h"
#include "net/inet_addr.hpp"
#include "net/poller.hpp"
#include "net/socket.hpp"

namespace falconlink {

TEST(ConnectionTest, EventTest) {
  InetAddr local_host("127.0.0.1", 20080);
  auto server_sock = std::make_unique<Socket>();
  server_sock->bind(local_host);
  server_sock->listen();
  Connection server_conn(std::move(server_sock));
  ASSERT_NE(server_conn.getSocket(), nullptr);

  server_conn.setEvents(POLL_ADD | POLL_ET);
  EXPECT_NE(server_conn.getEvents() & POLL_ADD, 0);
  EXPECT_NE(server_conn.getEvents() & POLL_ET, 0);
  server_conn.setRevents(POLL_READ);
  EXPECT_NE(server_conn.getRevents() & POLL_READ, 0);
}

TEST(ConnectionTest, CallbackTest) {
  InetAddr local_host("127.0.0.1", 20080);
  auto server_sock = std::make_unique<Socket>();
  server_sock->bind(local_host);
  server_sock->listen();
  Connection server_conn(std::move(server_sock));
  ASSERT_NE(server_conn.getSocket(), nullptr);

  server_conn.setCallback([](Connection *) -> void {});
  int i = 0;
  server_conn.setCallback([&target = i](Connection *) -> void { target += 1; });
  server_conn.getCallback()();
  EXPECT_EQ(i, 1);
}

TEST(ConnectionTest, ConnectTest) {
  InetAddr local_host("127.0.0.1", 20080);
  auto server_sock = std::make_unique<Socket>();
  server_sock->bind(local_host);
  server_sock->listen();
  Connection server_conn(std::move(server_sock));
  ASSERT_NE(server_conn.getSocket(), nullptr);

  const char *client_message = "hello from client";
  const char *server_message = "hello from server";
  std::thread client_thread([&]() {
    // build a client connecting with server
    auto client_sock = std::make_unique<Socket>();
    client_sock->connect(local_host);
    Connection client_conn(std::move(client_sock));
    // send a message to server
    client_conn.WriteToWriteBuffer(client_message);
    EXPECT_EQ(client_conn.GetWriteBufferSize(), strlen(client_message));
    client_conn.send();
    // recv a message from server
    auto [read, exit] = client_conn.recv();
    ASSERT_TRUE(exit);
    EXPECT_EQ(read, strlen(server_message));
    EXPECT_EQ(client_conn.ReadAsString(), std::string(server_message));
  });

  client_thread.detach();
  InetAddr client_address;
  auto connected_sock =
      std::make_unique<Socket>(server_conn.getSocket()->accept(client_address));
  connected_sock->setNonBlock();
  EXPECT_NE(connected_sock->fd(), -1);
  Connection connected_conn(std::move(connected_sock));
  sleep(1);
  // recv a message from client
  auto [read, exit] = connected_conn.recv();
  ASSERT_FALSE(exit);
  EXPECT_EQ(read, strlen(client_message));
  EXPECT_EQ(connected_conn.GetReadBufferSize(), strlen(client_message));
  // send a message to client
  connected_conn.WriteToWriteBuffer(server_message);
  connected_conn.send();
}

}  // namespace falconlink
