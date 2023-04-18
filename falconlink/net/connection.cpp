#include "net/connection.hpp"

#include <sys/socket.h>
#include <cstring>

#include "common/buffer.hpp"
#include "common/logger.hpp"

namespace falconlink {

Connection::Connection(std::unique_ptr<Socket> socket)
    : socket_(std::move(socket)) {}

int Connection::fd() const { return socket_->fd(); }

Socket *Connection::getSocket() { return socket_.get(); }

void Connection::setEvents(uint32_t events) { events_ = events; }

uint32_t Connection::getEvents() const { return events_; }

void Connection::setRevents(uint32_t revents) { revents_ = revents; }

uint32_t Connection::getRevents() const { return revents_; }

void Connection::setCallback(
    const std::function<void(Connection *)> &callback) {
  callback_ = [callback, this] { return callback(this); };
}

std::function<void()> Connection::getCallback() { return callback_; }

auto Connection::FindAndPopTill(const std::string &target)
    -> std::optional<std::string> {
  return read_buf_.findAndPopTill(target);
}

auto Connection::GetReadBufferSize() const -> size_t {
  return read_buf_.size();
}

auto Connection::GetWriteBufferSize() const -> size_t {
  return write_buf_.size();
}

void Connection::WriteToReadBuffer(const unsigned char *buf, size_t size) {
  read_buf_.append(buf, size);
}

void Connection::WriteToWriteBuffer(const unsigned char *buf, size_t size) {
  write_buf_.append(buf, size);
}

void Connection::WriteToReadBuffer(const std::string &str) {
  read_buf_.append(str);
}

void Connection::WriteToWriteBuffer(const std::string &str) {
  write_buf_.append(str);
}

void Connection::WriteToWriteBuffer(std::vector<unsigned char> &&other_buf) {
  write_buf_.append(std::move(other_buf));
}

const unsigned char *Connection::Read() { return read_buf_.data(); }

std::string Connection::ReadAsString() const {
  auto str_view = read_buf_.toStringView();
  return {str_view.begin(), str_view.end()};
}

auto Connection::recv() -> std::pair<ssize_t, bool> {
  // read all available bytes, since Edge-trigger
  int from_fd = fd();
  ssize_t read = 0;
  unsigned char buf[TEMP_BUF_SIZE + 1];
  memset(buf, 0, sizeof(buf));
  while (true) {
    ssize_t curr_read = ::recv(from_fd, buf, TEMP_BUF_SIZE, 0);
    if (curr_read > 0) {
      read += curr_read;
      WriteToReadBuffer(buf, curr_read);
      memset(buf, 0, sizeof(buf));
    } else if (curr_read == 0) {
      // the client has exit
      return {read, true};
    } else if (curr_read == -1 && errno == EINTR) {
      // normal interrupt
      continue;
    } else if (curr_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      // all data read
      break;
    } else {
      LOG_ERROR("HandleConnection: recv() error");
      return {read, true};
    }
  }
  return {read, false};
}

void Connection::send() {
  // robust write
  ssize_t curr_write = 0;
  ssize_t write;
  const ssize_t to_write = GetWriteBufferSize();
  const unsigned char *buf = write_buf_.data();
  while (curr_write < to_write) {
    write = ::send(fd(), buf + curr_write, to_write - curr_write, 0);
    if (write <= 0) {
      if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
        LOG_ERROR("Error in Connection::Send()");
        clearWriteBuffer();
        return;
      }
      write = 0;
    }
    curr_write += write;
  }
  clearWriteBuffer();
}

void Connection::clearReadBuffer() { read_buf_.clear(); }

void Connection::clearWriteBuffer() { write_buf_.clear(); }

void Connection::setEventLoop(EventLoop *looper) { owner_looper_ = looper; }

EventLoop *Connection::getEventLoop() { return owner_looper_; }

}  // namespace falconlink
