#include "http/response.hpp"

#include <sstream>
#include <utility>

#include "http/header.hpp"
#include "http/http_utils.hpp"

namespace falconlink {

namespace http {

Response Response::Make200Response(bool should_close,
                                   std::optional<std::string> resource_url) {
  return {RESPONSE_OK, should_close, std::move(resource_url)};
}

Response Response::Make400Response() {
  return {RESPONSE_BAD_REQUEST, true, std::nullopt};
}

Response Response::Make404Response() {
  return {RESPONSE_NOT_FOUND, true, std::nullopt};
}

Response Response::Make503Response() {
  return {RESPONSE_SERVICE_UNAVAILABLE, true, std::nullopt};
}

Response::Response(const std::string &status_code, bool should_close,
                   std::optional<std::string> resource_url)
    : should_close_(should_close), resource_url_(std::move(resource_url)) {
  // construct the status line
  std::stringstream str_stream;
  str_stream << HTTP_VERSION << SPACE << status_code;
  status_line_ = str_stream.str();
  // add necessary headers
  headers_.emplace_back(HEADER_SERVER, SERVER_FALCONLINK);
  headers_.emplace_back(
      HEADER_CONNECTION,
      ((should_close_) ? CONNECTION_CLOSE : CONNECTION_KEEP_ALIVE));
  // if resource is specified and available
  if (resource_url_.has_value() && isFileExists(resource_url_.value())) {
    size_t content_length = getFileSize(resource_url_.value());
    headers_.emplace_back(HEADER_CONTENT_LENGTH,
                          std::to_string(content_length));
    // parse out the extension
    auto last_dot = resource_url_.value().find_last_of(DOT);
    if (last_dot != std::string::npos) {
      auto extension_raw_str = resource_url_.value().substr(last_dot + 1);
      auto extension = toExtension(extension_raw_str);
      headers_.emplace_back(HEADER_CONTENT_TYPE, ExtensionToMime(extension));
    }
  } else {
    resource_url_ = std::nullopt;
    headers_.emplace_back(HEADER_CONTENT_LENGTH, CONTENT_LENGTH_ZERO);
  }
}

void Response::serialize(std::vector<unsigned char> &buffer) {  // NOLINT
  // construct everything before body
  std::stringstream str_stream;
  str_stream << status_line_ << CRLF;
  for (const auto &header : headers_) {
    str_stream << header.serialize();
  }
  str_stream << CRLF;
  std::string response_head = str_stream.str();
  buffer.insert(buffer.end(), response_head.begin(), response_head.end());
}

std::vector<Header> Response::getHeaders() { return headers_; }

bool Response::changeHeader(const std::string &key,
                            const std::string &new_value) {
  for (auto &it : headers_) {
    if (it.key() == key) {
      it.setValue(new_value);
      return true;
    }
  }
  return false;
}

}  // namespace http

}  // namespace falconlink
