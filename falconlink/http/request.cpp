#include "http/request.hpp"

#include <algorithm>

#include "http/header.hpp"
#include "http/http_utils.hpp"

namespace falconlink {

namespace http {

Request::Request(Method method, Version version, std::string resource_url,
                 const std::vector<Header> &headers) noexcept
    : method_(method),
      version_(version),
      resource_url_(std::move(resource_url)),
      headers_(headers),
      is_valid_(true) {}

Request::Request(const std::string &request_str) noexcept {
  auto lines = split(request_str, CRLF);
  if (lines.size() < 2 || !lines.back().empty()) {
    invalid_reason_ = "Request format is wrong.";
    return;
  }
  /* the ending of a request should be '\r\n\r\n'
   * which is split to empty string in the last token */
  if (!lines.back().empty()) {
    invalid_reason_ = "Ending of the request is not \\r\\n\\r\\n";
    return;
  }
  lines.pop_back();
  bool request_line_parse_success = parseRequestLine(lines[0]);
  if (!request_line_parse_success) {
    return;
  }

  lines.erase(lines.begin());
  for (const auto &line : lines) {
    Header header{line};
    if (!header.valid()) {
      invalid_reason_ = "Fail to parse header line: " + line;
      return;
    }
    scanHeader(header);
    headers_.push_back(std::move(header));
  }
  is_valid_ = true;
}

auto Request::shouldClose() const noexcept -> bool { return should_close_; }

auto Request::valid() const noexcept -> bool { return is_valid_; }

auto Request::getMethod() const noexcept -> Method { return method_; }

auto Request::getVersion() const noexcept -> Version { return version_; }

auto Request::getResourceUrl() const noexcept -> std::string {
  return urlDecode(resource_url_);
}

auto Request::getHeaders() const noexcept -> std::vector<Header> {
  return headers_;
}

std::string Request::getInvalidReason() const { return invalid_reason_; }

bool Request::parseRequestLine(const std::string &request_line) {
  auto tokens = split(request_line, SPACE);
  if (tokens.size() != 3) {
    invalid_reason_ = "Invalid first request headline: " + request_line;
    return false;
  }
  method_ = toMethod(tokens[0]);
  if (method_ == Method::UNSUPPORTED) {
    invalid_reason_ = "Unsupported method: " + tokens[0];
    return false;
  }
  version_ = toVersion(tokens[2]);
  if (version_ == Version::UNSUPPORTED) {
    invalid_reason_ = "Unsupported version: " + tokens[2];
    return false;
  }
  // default route to index.html
  resource_url_ = (tokens[1].empty() || tokens[1].back() == '/')
                      ? tokens[1] + DEFAULT_ROUTE
                      : tokens[1];
  return true;
}

void Request::scanHeader(const Header &header) {
  /* currently only scan for whether the connection should be closed after
   * service */
  auto key = format(header.key());
  if (key == format(HEADER_CONNECTION)) {
    auto value = format(header.value());
    if (value == format(CONNECTION_KEEP_ALIVE)) {
      should_close_ = false;
    }
  }
}

auto operator<<(std::ostream &os, const Request &request) -> std::ostream & {
  if (!request.valid()) {
    os << "Request is not invalid." << std::endl;
    os << "Reason: " << request.invalid_reason_ << std::endl;
  } else {
    os << "Request is valid." << std::endl;
    os << "Method: " << METHOD_TO_STRING.at(request.method_) << std::endl;
    os << "HTTP Version: " << VERSION_TO_STRING.at(request.version_)
       << std::endl;
    os << "Resource Url: " << request.resource_url_ << std::endl;
    os << "Connection Keep Alive: "
       << ((request.should_close_) ? "False" : "True") << std::endl;
    os << "Headers: " << std::endl;
    auto headers = request.getHeaders();
    std::for_each(headers.begin(), headers.end(),
                  [&](auto header) { os << header.serialize(); });
  }
  return os;
}

}  // namespace http

}  // namespace falconlink
