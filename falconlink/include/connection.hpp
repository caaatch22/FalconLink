#pragma once
#include <functional>
#include "buffer.hpp"
#include "channel.hpp"

namespace falconlink {

class EventLoop;
class Socket;
class Connection;
class Buffer;

class Connection {
 public:
  enum class State { 
    Invalid = 1,
    Connecting = 2,
    Connected = 3,
    Closed = 4,
    Failed = 5,
  };

  Connection(EventLoop *loop, Socket *sock);

  // disallow copy and move
  Connection(const Connection &rhs) = delete;
  Connection &operator=(const Connection &rhs) = delete;
  Connection(Connection &&rhs) = delete;
  Connection &operator=(Connection &&rhs) = delete;

  ~Connection();
    
  void read();
  void write();
  void send(std::string msg);

  void setDeleteConnectionCallback(const std::function<void(Socket *)> &cb);
  void setOnConnectCallback(const std::function<void(Connection *)> &cb);
  void setOnMessageCallback(const std::function<void(Connection *)> &cb);
  void business();

  State getState() const;
  void close();
  void setSendBuffer(const char *str);
  Buffer *getReadBuffer();
  const char *readBuffer();
  Buffer *getSendBuffer();
  const char *sendBuffer();
  void getlineSendBuffer();
  Socket *getSocket();

  // void onConnect(std::function<void()> fn);
  // void onMessage(std::function<void()> fn);

 private:
  EventLoop *loop_;
  Socket *sock_;
  Channel *channel_{nullptr};
  State state_{State::Invalid};
  Buffer *read_buf_{nullptr};
  Buffer *send_buf_{nullptr};
  std::function<void(Socket *)> delete_connectioin_callback_;

  std::function<void(Connection *)> on_connect_callback_;
  std::function<void(Connection *)> on_message_callback_;

  void readNonBlock();
  void writeNonBlock();

  /**
   *@brief Never used by server, only for client;
   */
  void readBlock();
  void writeBlock();
};

} // namespace falconlink

