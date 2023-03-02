#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>

#include "../../falconlink/include/util.hpp"
#include "../../falconlink/include/poller.hpp"
#include "../../falconlink/include/inet_addr.hpp"
#include "../../falconlink/include/socket.hpp"
#include "../../falconlink/include/event_loop.hpp"
#include "../../falconlink/include/channel.hpp"
#include "../../falconlink/include/acceptor.hpp"

#define READ_BUFFER 1024

using namespace falconlink;

class Server {
 public:
  Server(EventLoop *loop) :loop_(loop) {
    acceptor_ = new Acceptor(loop);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor_->setNewConnectionCallback(cb);
  }

  ~Server() { delete acceptor_; };

  void handleReadEvent(int sockfd) {
    char buf[READ_BUFFER];
    while (true) {
      // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
      bzero(&buf, sizeof(buf));
      ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
      if (bytes_read > 0) {
        printf("message from client fd %d: %s\n", sockfd, buf);
        write(sockfd, buf, sizeof(buf));
      } else if (bytes_read == -1 &&
                 errno == EINTR) {  // 客户端正常中断、继续读取
        printf("continue reading");
        continue;
      } else if (bytes_read == -1 &&
                 ((errno == EAGAIN) ||
                  (errno ==
                   EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
        printf("finish reading once, errno: %d\n", errno);
        break;
      } else if (bytes_read == 0) {  // EOF，客户端断开连接
        printf("EOF, client fd %d disconnected\n", sockfd);
        close(sockfd);  // 关闭socket会自动将文件描述符从epoll树上移除
        break;
      }
    }
  }

  void newConnection(Socket *serv_sock) {
    InetAddr *clnt_addr = new InetAddr();  // memory leak!
    Socket *clnt_sock =
        new Socket(serv_sock->accept(clnt_addr));  // memory leak!
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->fd(),
           inet_ntoa(clnt_addr->addr_.sin_addr),
           ntohs(clnt_addr->addr_.sin_port));
    clnt_sock->setNonBlock();
    Channel *clntChannel = new Channel(loop_, clnt_sock->fd());
    std::function<void()> cb = std::bind(&Server::handleReadEvent, this, clnt_sock->fd());
    clntChannel->setCallback(cb);
    clntChannel->enableReading();
  }

 private:
  EventLoop *loop_;
  Acceptor *acceptor_;
};

int main() {

  EventLoop *loop = new EventLoop();
  Server *server = new Server(loop);
  loop->loop();

  delete loop;
  delete server;

  return 0;
}

// void handleReadEvent(int sockfd){
//   char buf[READ_BUFFER];
//   while (
//       true) {  // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
//     bzero(&buf, sizeof(buf));
//     ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
//     if (bytes_read > 0) {
//       printf("message from client fd %d: %s\n", sockfd, buf);
//       write(sockfd, buf, sizeof(buf));
//     } else if (bytes_read == -1 && errno == EINTR) {  // 客户端正常中断、继续读取
//       printf("continue reading");
//       continue;
//     } else if (bytes_read == -1 &&
//                ((errno == EAGAIN) ||
//                 (errno ==
//                  EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
//       printf("finish reading once, errno: %d\n", errno);
//       break;
//     } else if (bytes_read == 0) {  // EOF，客户端断开连接
//       printf("EOF, client fd %d disconnected\n", sockfd);
//       close(sockfd);  // 关闭socket会自动将文件描述符从Poller树上移除
//       break;
//     }
//   }
// }
