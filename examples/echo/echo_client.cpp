#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <memory>

#include "net/connection.hpp"
#include "net/inet_addr.hpp"
#include "net/socket.hpp"
#include "common/thread_pool.hpp"

#define BUF_SIZE 2048

namespace falconlink {
class EchoClient {
 public:
  explicit EchoClient(InetAddr server_address) {
    auto client_socket = std::make_unique<Socket>();
    client_socket->connect(server_address);
    client_connection = std::make_unique<Connection>(std::move(client_socket));
  }

  void Begin() {
    char buf[BUF_SIZE + 1];
    memset(buf, 0, sizeof(buf));
    int fd = client_connection->fd();
    while (true) {
      // scan from user keyboard
      auto actual_read = ::read(STDIN_FILENO, buf, BUF_SIZE);
      send(fd, buf, actual_read, 0);
      memset(buf, 0, sizeof(buf));
      // echo back to screen from server's message
      auto actual_recv = recv(fd, buf, BUF_SIZE, 0);
      write(STDOUT_FILENO, buf, actual_recv);
      memset(buf, 0, sizeof(buf));
    }
  }

 private:
  std::unique_ptr<Connection> client_connection;
};
}  // namespace falconlink

int main() {
  falconlink::InetAddr local_address("0.0.0.0", 20080);
  falconlink::EchoClient echo_client(local_address);
  echo_client.Begin();

  return 0;
}
