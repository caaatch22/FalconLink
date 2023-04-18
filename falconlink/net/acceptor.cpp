#include "net/acceptor.hpp"

#include <memory>

#include "net/inet_addr.hpp"
#include "net/poller.hpp"
#include "net/socket.hpp"
#include "common/logger.hpp"

namespace falconlink {

Acceptor::Acceptor(EventLoop *main_reactor_, const InetAddr &server_addr,
                   std::vector<EventLoop *> sub_reactors)
    : sub_reactors_(std::move(sub_reactors)) {
  auto acceptor_sock = std::make_unique<Socket>();
  acceptor_sock->bind(server_addr);
  acceptor_sock->listen();

  acceptor_conn_ = std::make_unique<Connection>(std::move(acceptor_sock));
  acceptor_conn_->setEvents(POLL_READ);  // not edge-trigger for listener
  acceptor_conn_->setEventLoop(main_reactor_);

  main_reactor_->addAcceptor(acceptor_conn_.get());
  setAcceptCallback([](Connection *) {});
  setHandleCallback([](Connection *) {});
}

void Acceptor::basicAcceptBehavior(Connection *server_conn) {
  InetAddr client_address;
  int accept_fd = server_conn->getSocket()->accept(client_address);
  if (accept_fd == -1) {
    return;
  }
  auto client_sock = std::make_unique<Socket>(accept_fd);
  client_sock->setNonBlock();
  auto client_connection = std::make_unique<Connection>(std::move(client_sock));
  client_connection->setEvents(POLL_READ | POLL_ET);  // edge-trigger for client
  client_connection->setCallback(getHandleCallback());
  // randomized distribution. uniform in long term.
  int idx = rand() % sub_reactors_.size();  // NOLINT
  LOG_INFO("new client fd = " + std::to_string(client_connection->fd()) +
           " maps to reactor " + std::to_string(idx));
  client_connection->setEventLoop(sub_reactors_[idx]);
  sub_reactors_[idx]->addConnection(std::move(client_connection));
}

void Acceptor::setAcceptCallback(
    std::function<void(Connection *)> accept_callback) {
  accept_callback_ = std::move(accept_callback);
  acceptor_conn_->setCallback([this](auto &&PH1) {
    basicAcceptBehavior(std::forward<decltype(PH1)>(PH1));
    accept_callback_(std::forward<decltype(PH1)>(PH1));
  });
}

void Acceptor::setHandleCallback(
    std::function<void(Connection *)> handle_callback) {
  handle_callback_ = std::move(handle_callback);
}

std::function<void(Connection *)> Acceptor::getAcceptCallback() const {
  return accept_callback_;
}

std::function<void(Connection *)> Acceptor::getHandleCallback() const {
  return handle_callback_;
}

Connection *Acceptor::getAcceptorConnection() { return acceptor_conn_.get(); }

}  // namespace falconlink
