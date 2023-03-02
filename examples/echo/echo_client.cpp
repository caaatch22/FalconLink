#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "../../falconlink/include/socket.hpp"
#include "../../falconlink/include/util.hpp"
#include "../../falconlink/include/socket.hpp"
#include "../../falconlink/include/inet_addr.hpp"

#define BUFFER_SIZE 1024

using namespace falconlink;

int main() {
  Socket sock;
  InetAddr addr("127.0.0.1", 8888);

  sock.connect(addr);

  while (true) {
    char buf[BUFFER_SIZE];
    bzero(&buf, sizeof(buf));
    scanf("%s", buf);
    ssize_t write_bytes = write(sock.fd(), buf, sizeof(buf));
    if (write_bytes == -1) {
      printf("socket already disconnected, can't write any more!\n");
      break;
    }
    bzero(&buf, sizeof(buf));
    ssize_t read_bytes = read(sock.fd(), buf, sizeof(buf));
    if (read_bytes > 0) {
      printf("message from server: %s\n", buf);
    } else if (read_bytes == 0) {
      printf("server socket disconnected!\n");
      break;
    } else if (read_bytes == -1) {
      errif(true, "socket read error");
    }
  }
  
  return 0;
}
