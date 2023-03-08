#pragma once

#include <optional>
#include <string>
#include <vector>

namespace falconlink {

namespace http {


class Header;

/**
 * The HTTP Response class
 * use vector of char to be able to contain binary data
 */
class Response {
 public:
  /* 200 OK response */
  static Response Make200Response(bool should_close,
                                  std::optional<std::string> resource_url);
  /* 400 Bad Request response, close connection */
  static Response Make400Response();
  /* 404 Not Found response, close connection */
  static Response Make404Response();
  /* 503 Service Unavailable response, close connection */
  static Response Make503Response();

  Response(const std::string &status_code, bool should_close,
           std::optional<std::string> resource_url);

  /* no content, content should separately be loaded */
  void serialize(std::vector<unsigned char> &buffer);  // NOLINT

  std::vector<Header> getHeaders();

  bool changeHeader(const std::string &key, const std::string &new_value);

 private:
  std::string status_line_;

  bool should_close_;

  std::vector<Header> headers_;

  std::optional<std::string> resource_url_;

  std::vector<unsigned char> body_;
};

}  // namespace http

}  // namespace falconlink
