#include "net/event_loop.hpp"

#include <unistd.h>

#include <atomic>
#include <memory>
#include <numeric>
#include <thread>  // NOLINT
#include <vector>

#include "gtest/gtest.h"
#include "net/connection.hpp"
#include "net/inet_addr.hpp"
#include "net/poller.hpp"
#include "net/socket.hpp"

namespace falconlink {

TEST(EventLoopTest, EventTest) {
  EventLoop looper;
  // build the server socket
  InetAddr local_host("127.0.0.1", 20080);
  Socket server_sock;
  server_sock.bind(local_host);
  server_sock.listen();
  ASSERT_NE(server_sock.fd(), -1);

  int client_num = 3;
  std::vector<std::thread> threads;
  for (int i = 0; i < client_num; i++) {
    threads.emplace_back([&host = local_host]() {
      auto client_socket = Socket();
      client_socket.connect(host);
      sleep(1);
    });
  }

  // build 3 connections and add into looper with customized callback function
  std::vector<int> reach(client_num, 0);
  for (int i = 0; i < client_num; i++) {
    InetAddr client_address;
    auto client_sock =
        std::make_unique<Socket>(server_sock.accept(client_address));
    EXPECT_NE(client_sock->fd(), -1);
    client_sock->setNonBlock();
    auto client_conn = std::make_unique<Connection>(std::move(client_sock));
    client_conn->setEvents(POLL_READ);
    client_conn->setCallback(
        [&reach = reach, index = i](Connection *conn) { reach[index] = 1; });
    looper.addConnection(std::move(client_conn));
  }

  /* the looper execute each client's callback once, upon their exit */
  std::thread runner([&]() { looper.loop(); });
  sleep(2);
  looper.quit();

  /* each client's callback should have already been executed */
  EXPECT_EQ(std::accumulate(reach.begin(), reach.end(), 0), client_num);

  runner.join();
  for (int i = 0; i < client_num; i++) {
    threads[i].join();
  }
}

}  // namespace falconlink
