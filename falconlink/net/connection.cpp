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
    : loop_(loop), sock_(sock) {
  channel_ = new Channel(loop_, sock->fd());
  channel_->enableRead();
  channel_->useET();
  std::function<void()> cb = std::bind(&Connection::echo, this, sock->fd());
  channel_->setReadCallback(cb);
}

Connection::~Connection() {
  delete channel_;
  delete sock_;
}

void Connection::echo(int sockfd){
  char buf[1024];
  while (
      true) {  // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
    memset(buf, 0, sizeof buf);
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      read_buf_.append(buf, bytes_read);
    } else if (bytes_read == -1 && errno == EINTR) {  // 客户端正常中断、继续读取
      printf("continue reading\n");
      continue;
    } else if (bytes_read == -1 &&
               ((errno == EAGAIN) ||
                (errno ==
                 EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
      printf("message from client fd %d: %s\n", sockfd, read_buf_.c_str());
      // errif(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1,
      // "socket write error");
      send(sockfd);
      read_buf_.clear();
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("EOF, client fd %d disconnected\n", sockfd);
      deleteConnectionCallback(sockfd);  // 多线程会有bug
      break;
    } else {
      printf("Connection reset by peer\n");
      deleteConnectionCallback(sockfd);  // 会有bug，注释后单线程无bug
      break;
    }
  }
}

void Connection::send(int sockfd){
  char buf[read_buf_.size()];
  strcpy(buf, read_buf_.c_str());
  int data_size = read_buf_.size();
  int data_left = data_size;
  while (data_left > 0) {
    ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
    if (bytes_write == -1 && errno == EAGAIN) {
      break;
    }
    data_left -= bytes_write; 
  }
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> cb) {
  deleteConnectionCallback = cb;
}

}  // namespace falconlink
