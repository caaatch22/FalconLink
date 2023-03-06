#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <unordered_map>
#include <vector>
#include <iostream>

#include "../../falconlink/include/falconlink.hpp"

using falconlink::Connection;
using falconlink::EventLoop;
using falconlink::Server;
using falconlink::Signal;
using falconlink::Socket;

int main() {
  EventLoop *loop = new EventLoop();
  Server *server = new Server(loop);

  Signal::signal(SIGINT, [&] {
    delete server;
    delete loop;
    std::cout << "\nServer exit!" << std::endl;
    exit(0);
  });

  server->newConnect([](Connection *conn) {
    std::cout << "New connection fd: " << conn->getSocket()->fd() << std::endl;
  });

  server->onMessage([](Connection *conn) {
    std::cout << "Message from client: " << conn->readBuffer() << std::endl;
    if (conn->getState() == Connection::State::Connected) {
      conn->send(conn->readBuffer());
    }
  });
  loop->loop();
  return 0;
}
