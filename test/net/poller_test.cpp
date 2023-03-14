#include "net/poller.hpp"

#include <unistd.h>

#include <memory>
#include <thread>  // NOLINT
#include <vector>

#include "gtest/gtest.h"
#include "net/connection.hpp"
#include "net/inet_addr.hpp"
#include "net/socket.hpp"

namespace falconlink {

TEST(PollerTest, BasicTest) {
  InetAddr local_host("127.0.0.1", 20080);
  Socket server_sock;

  // build the server socket
  server_sock.bind(local_host);
  server_sock.listen();
  ASSERT_NE(server_sock.fd(), -1);

  int client_num = 3;
  // build the empty poller
  Poller poller(client_num);
  ASSERT_EQ(poller.getPollSize(), client_num);

  {
    std::vector<std::thread> threads;
    for (int i = 0; i < client_num; i++) {
      threads.emplace_back([&]() {
        auto client_socket = Socket();
        client_socket.connect(local_host);
        char message[] = "Hello from client!";
        send(client_socket.fd(), message, strlen(message), 0);
        sleep(2);
      });
    }

    // server accept clients and build connection for them
    std::vector<std::shared_ptr<Connection>> client_conns;
    for (int i = 0; i < client_num; i++) {
      InetAddr client_address;
      auto client_sock =
          std::make_unique<Socket>(server_sock.accept(client_address));
      EXPECT_NE(client_sock->fd(), -1);
      client_conns.push_back(
          std::make_shared<Connection>(std::move(client_sock)));
      client_conns[i]->setEvents(POLL_READ);
    }

    // each client connection under poller's monitor
    for (int i = 0; i < client_num; i++) {
      poller.addConnection(client_conns[i].get());
    }
    sleep(1);
    auto ready_conns = poller.poll();
    EXPECT_EQ(ready_conns.size(), client_num);

    for (int i = 0; i < client_num; i++) {
      threads[i].join();
    }
  }
}

}  // namespace falconlink
