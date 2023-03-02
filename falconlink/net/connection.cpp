#include "../include/connection.hpp"

#include <string.h>
#include <unistd.h>

#include <functional>

#include "../include/buffer.hpp"
#include "../include/channel.hpp"
#include "../include/socket.hpp"
#include "../include/util.hpp"

namespace falconlink {

Connection::Connection(EventLoop *loop, Socket *sock)
    : loop_(loop), sock_(sock), channel_(nullptr) {
  channel_ = new Channel(loop_, sock->fd());
  std::function<void()> cb = std::bind(&Connection::echo, this, sock->fd());
  channel_->setCallback(cb);
  channel_->enableReading();
}

Connection::~Connection() {
  delete channel_;
  delete sock_;
}

void Connection::echo(int sockfd) {
  unsigned char buf[1024];
  while (true) {  // 由于使用非阻塞IO，多次读取buf大小数据，直到全部读取完毕
    memset(buf, 0, sizeof(buf));
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      read_buf_.append(buf, bytes_read);
    } else if (bytes_read == -1 &&
               errno == EINTR) {  // 客户端正常中断、继续读取
      printf("continue reading");
      continue;
    } else if (bytes_read == -1 &&
               ((errno == EAGAIN) ||
                (errno ==
                 EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
      printf("finish reading once\n");
      errif(write(sockfd, read_buf_.data(), read_buf_.size()) == -1,
            "socket write error");
      read_buf_.clear();
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("EOF, client fd %d disconnected\n", sockfd);
      // close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
      deleteConnectionCallback(sock_);
      break;
    }
  }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket *)> cb) {
  deleteConnectionCallback = cb;
}
}  // namespace falconlink
