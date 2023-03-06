#include "../include/connection.hpp"

#include <string.h>
#include <unistd.h>

#include <cassert>
#include <functional>

#include "../include/buffer.hpp"
#include "../include/channel.hpp"
#include "../include/socket.hpp"
#include "../include/util.hpp"

namespace falconlink {

Connection::Connection(EventLoop *loop, Socket *sock)
    : loop_(loop), sock_(sock) {
  if (loop_ != nullptr) {
    channel_ = new Channel(loop_, sock_);
    channel_->enableRead();
    channel_->useET();
  }
  read_buf_ = new Buffer();
  send_buf_ = new Buffer();
  state_ = State::Connected;
}

Connection::~Connection() {
  if (loop_ != nullptr) {
    delete channel_;
  }
  delete sock_;
  delete read_buf_;
  delete send_buf_;
}

void Connection::read() {
  assert(state_ == State::Connected && "connection state is disconnected!");
  read_buf_->clear();
  if (sock_->isNonBlock()) {
    readNonBlock();
  } else {
    readBlock();
  }
}

void Connection::write() {
  assert(state_ == State::Connected && "connection state is disconnected!");
  if (sock_->isNonBlock()) {
    writeNonBlock();
  } else {
    writeBlock();
  }
  send_buf_->clear();
}

void Connection::readNonBlock() {
  int sockfd = sock_->fd();
  char buf[1024];  // 这个buf大小无所谓
  while (true) {
    memset(buf, 0, sizeof(buf));
    ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      read_buf_->append(buf, bytes_read);
    } else if (bytes_read == -1 && errno == EINTR) {  // 程序正常中断、继续读取
      printf("continue reading\n");
      continue;
    } else if (bytes_read == -1 &&
               ((errno == EAGAIN) ||
                (errno ==
                 EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("read EOF, client fd %d disconnected\n", sockfd);
      state_ = State::Closed;
      close();
      break;
    } else {
      printf("Other error on client fd %d\n", sockfd);
      state_ = State::Closed;
      close();
      break;
    }
  }
}

void Connection::writeNonBlock() {
  int sockfd = sock_->fd();
  char buf[send_buf_->size()];
  memcpy(buf, send_buf_->c_str(), send_buf_->size());
  int data_size = send_buf_->size();
  int data_left = data_size;
  while (data_left > 0) {
    ssize_t bytes_write =
        ::write(sockfd, buf + data_size - data_left, data_left);
    if (bytes_write == -1 && errno == EINTR) {
      printf("continue writing\n");
      continue;
    }
    if (bytes_write == -1 && errno == EAGAIN) {
      break;
    }
    if (bytes_write == -1) {
      printf("Other error on client fd %d\n", sockfd);
      state_ = State::Closed;
      break;
    }
    data_left -= bytes_write;
  }
}

void Connection::readBlock() {
  int sockfd = sock_->fd();
  unsigned int rcv_size = 0;
  socklen_t len = sizeof(rcv_size);
  getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &len);
  char buf[rcv_size];
  ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
  if (bytes_read > 0) {
    read_buf_->append(buf, bytes_read);
  } else if (bytes_read == 0) {
    printf("read EOF, blocking client fd %d disconnected\n", sockfd);
    state_ = State::Closed;
  } else if (bytes_read == -1) {
    printf("Other error on blocking client fd %d\n", sockfd);
    state_ = State::Closed;
  }
}

void Connection::writeBlock() {
  // 没有处理send_buffer_数据大于TCP写缓冲区，的情况，可能会有bug
  int sockfd = sock_->fd();
  ssize_t bytes_write = ::write(sockfd, send_buf_->c_str(), send_buf_->size());
  if (bytes_write == -1) {
    printf("Other error on blocking client fd %d\n", sockfd);
    state_ = State::Closed;
  }
}

void Connection::send(std::string msg) {
  setSendBuffer(msg.c_str());
  write();
}

void Connection::business() {
  read();
  on_message_callback_(this);
}

void Connection::close() { delete_connectioin_callback_(sock_); }

Connection::State Connection::getState() const { return state_; }
void Connection::setSendBuffer(const char *str) { send_buf_->setBuf(str); }
// TODO(catch22): fix API
Buffer *Connection::getReadBuffer() { return read_buf_; }
const char *Connection::readBuffer() { return read_buf_->c_str(); }
Buffer *Connection::getSendBuffer() { return send_buf_; }
const char *Connection::sendBuffer() { return send_buf_->c_str(); }

void Connection::setDeleteConnectionCallback(
    std::function<void(Socket *)> const &callback) {
  delete_connectioin_callback_ = callback;
}
void Connection::setOnConnectCallback(
    std::function<void(Connection *)> const &callback) {
  on_connect_callback_ = callback;
  // channel_->SetReadCallback([this]() { on_connect_callback_(this); });
}

void Connection::setOnMessageCallback(
    std::function<void(Connection *)> const &callback) {
  on_message_callback_ = callback;
  std::function<void()> bus = std::bind(&Connection::business, this);
  channel_->setReadCallback(bus);
}

void Connection::getlineSendBuffer() { send_buf_->getline(); }

Socket *Connection::getSocket() { return sock_; }

}  // namespace falconlink
