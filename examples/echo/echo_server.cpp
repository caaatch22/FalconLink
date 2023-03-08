#include "net/server.hpp"

int main() {
  falconlink::InetAddr local_address("0.0.0.0", 20080);
  falconlink::Server echo_server(local_address);
  echo_server
      .onHandle([&](falconlink::Connection* client_conn) {
        int from_fd = client_conn->fd();
        auto [read, exit] = client_conn->recv();
        if (exit) {
          client_conn->getEventLoop()->deleteConnection(from_fd);
          // client_conn ptr is invalid below here, do not touch it again
          return;
        }
        if (read) {
          client_conn->WriteToWriteBuffer(client_conn->ReadAsString());
          client_conn->send();
          client_conn->clearReadBuffer();
        }
      })
      .start();
  return 0;
}
