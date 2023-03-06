#include <iostream>

#include "../../falconlink/include/connection.hpp"
#include "../../falconlink/include/socket.hpp"

using falconlink::Buffer;
using falconlink::Connection;
using falconlink::InetAddr;
using falconlink::Socket;

int main() {
  Socket *sock = new Socket();
  sock->connect("127.0.0.1", 8888);

  Connection *conn = new Connection(nullptr, sock);

  while (true) {
    conn->getlineSendBuffer();
    conn->write();
    if (conn->getState() == Connection::State::Closed) {
      conn->close();
      break;
    }
    conn->read();
    std::cout << "Message from server: " << conn->readBuffer() << std::endl;
  }

  delete sock;
  delete conn;
  return 0;
}
