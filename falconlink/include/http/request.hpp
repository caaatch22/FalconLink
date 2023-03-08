#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "common/macros.hpp"

namespace falconlink {

namespace http {

class Header;
enum class Method;
enum class Version;

/**
 * The (limited GET/HEAD-only HTTP 1.1) HTTP Request class
 * it contains necessary request line features including method, resource url,
 * http version and since we supports http 1.1, it also cares if the client
 * connection should be kept alive
 */
class Request {
 public:
  Request(Method method, Version version, std::string resource_url,
          const std::vector<Header> &headers) noexcept;
  explicit Request(
      const std::string &request_str) noexcept;  // deserialize method
  NON_COPYABLE(Request);
  auto isValid() const noexcept -> bool;
  auto shouldClose() const noexcept -> bool;
  std::string getInvalidReason() const;
  auto getMethod() const noexcept -> Method;
  auto getVersion() const noexcept -> Version;
  auto getResourceUrl() const noexcept -> std::string;
  auto getHeaders() const noexcept -> std::vector<Header>;
  friend auto operator<<(std::ostream &os, const Request &request) -> std::ostream &;

 private:
  auto parseRequestLine(const std::string &request_line) -> bool;
  void scanHeader(const Header &header);
  Method method_;
  Version version_;
  std::string resource_url_;
  std::vector<Header> headers_;
  bool should_close_{true};
  bool is_valid_{false};
  std::string invalid_reason_;
};

}  // namespace http

}  // namespace falconlink
