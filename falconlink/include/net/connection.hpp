#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "socket.hpp"
#include "event_loop.hpp"

#include "common/buffer.hpp"
#include "common/macros.hpp"

namespace falconlink {

constexpr int TEMP_BUF_SIZE = 2048;

class EventLoop;

/**
 * This Connection class encapsulates a TCP client connection
 * It could be set a custom callback function when new messages arrive
 * and it contains information about the monitoring events and return events
 * so that Poller could manipulate and epoll based on this Connection class
 */
class Connection {
 public:
  explicit Connection(std::unique_ptr<Socket> socket);
  ~Connection() = default;

  NON_COPYABLE(Connection);

  int fd() const;
  Socket *getSocket();

  /** for Poller */
  void setEvents(uint32_t events);
  uint32_t getEvents() const;
  void setRevents(uint32_t revents);
  uint32_t getRevents() const;

  void setCallback(const std::function<void(Connection *)> &callback);
  std::function<void()> getCallback();

  /** for Buffer */
  auto FindAndPopTill(const std::string &target) -> std::optional<std::string>;
  auto GetReadBufferSize() const -> size_t;
  auto GetWriteBufferSize() const -> size_t;
  void WriteToReadBuffer(const unsigned char *buf, size_t size);
  void WriteToWriteBuffer(const unsigned char *buf, size_t size);
  void WriteToReadBuffer(const std::string &str);
  void WriteToWriteBuffer(const std::string &str);
  void WriteToWriteBuffer(std::vector<unsigned char> &&other_buf);
  void clearReadBuffer();
  void clearWriteBuffer();

  const unsigned char *Read();
  std::string ReadAsString() const;

  /* return std::pair<How many bytes read, whether the client exits> */
  auto recv() -> std::pair<ssize_t, bool>;
  void send();

  void setEventLoop(EventLoop *looper);
  EventLoop *getEventLoop();

 private:
  EventLoop *owner_looper_{nullptr};

  std::unique_ptr<Socket> socket_;

  Buffer read_buf_;
  Buffer write_buf_;

  uint32_t events_{0};
  uint32_t revents_{0};

  std::function<void()> callback_{nullptr};
};

}  // namespace falconlink
