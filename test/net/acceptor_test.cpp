#include "net/acceptor.hpp"

#include <unistd.h>

#include <future>  // NOLINT
#include <memory>
#include <vector>

#include "gtest/gtest.h"
#include "net/connection.hpp"
#include "net/event_loop.hpp"
#include "net/inet_addr.hpp"
#include "net/poller.hpp"
#include "net/socket.hpp"
#include "common/thread_pool.hpp"

namespace falconlink {

TEST(AcceptorTest, FunctionTest) {

  InetAddr local_host("127.0.0.1", 8888);
  ThreadPool pool;

  // built an acceptor will one listener looper and one reactor together
  auto single_reactor = std::make_unique<EventLoop>();

  std::vector<EventLoop *> raw_reactors = {single_reactor.get()};
  auto acceptor = Acceptor(single_reactor.get(), local_host, raw_reactors);

  EXPECT_NE(acceptor.getAcceptorConnection()->fd(), -1);
  {
    int client_num = 3;
    std::atomic<int> accept_trigger = 0;
    std::atomic<int> handle_trigger = 0;

    // set acceptor customize functions
    acceptor.setAcceptCallback([&](Connection *) { accept_trigger++; });
    acceptor.setHandleCallback([&](Connection *) { handle_trigger++; });

    // start three clients and connect with server
    const char *msg = "Hello from client!";
    std::vector<std::future<void>> futs;
    for (int i = 0; i < client_num; i++) {
      auto fut = std::async(std::launch::async, [&]() {
        Socket client_sock;
        client_sock.connect(local_host);
        ASSERT_NE(client_sock.fd(), -1);
        send(client_sock.fd(), msg, strlen(msg), 0);
      });
      futs.push_back(std::move(fut));
    }

    auto runner = std::async(std::launch::async, [&]() { single_reactor->loop(); });
    futs.push_back(std::move(runner));
    sleep(2);
    single_reactor->quit();  // terminate the looper

    // accept & handle should be triggered exactly 3 times
    EXPECT_EQ(accept_trigger, client_num);
    EXPECT_TRUE(handle_trigger >= client_num);

    for (auto &f : futs) {
      f.wait();
    }
  }


}

}  // namespace falconlink
