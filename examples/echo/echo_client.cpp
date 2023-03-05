#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "../../falconlink/include/buffer.hpp"
#include "../../falconlink/include/socket.hpp"
#include "../../falconlink/include/util.hpp"
#include "../../falconlink/include/inet_addr.hpp"

using falconlink::Buffer;
using falconlink::InetAddr;
using falconlink::Socket;

int main() {
  Socket *sock = new Socket();
  sock->connect("127.0.0.1", 8888);

  int sockfd = sock->fd();

  Buffer *send_buf = new Buffer();
  Buffer *read_buf = new Buffer();

  while (true) {
    send_buf->getline();
    ssize_t write_bytes =
        write(sockfd, send_buf->c_str(), send_buf->size());
    if (write_bytes == -1) {
      printf("socket already disconnected, can't write any more!\n");
      break;
    }
    int already_read = 0;
    char buf[1024];  // 这个buf大小无所谓
    while (true) {
      memset(buf, 0, sizeof buf);
      ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
      if (read_bytes > 0) {
        read_buf->append(buf, read_bytes);
        already_read += read_bytes;
      } else if (read_bytes == 0) {  // EOF
        printf("server disconnected!\n");
        exit(EXIT_SUCCESS);
      }
      if (already_read >= send_buf->size()) {
        printf("message from server: %s\n", read_buf->c_str());
        break;
      }
    }
    read_buf->clear();
    }
    delete sock;
    return 0;
}

