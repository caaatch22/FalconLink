#include "http/header.hpp"

#include "http/http_utils.hpp"

namespace falconlink {

namespace http {


Header::Header(const std::string &key, const std::string &value)
    : key_(key), value_(value) {}

Header::Header(std::string &&key, std::string &&value)
    : key_(std::move(key)), value_(std::move(value)) {}

Header::Header(const std::string &line) {
  auto tokens = split(line, COLON);
  if (tokens.size() < 2) {
    valid_ = false;
    return;
  }
  key_ = std::move(tokens[0]);
  tokens.erase(tokens.begin());
  /* the value could be like '127.0.0.1:20080' and get split into more than 1 */
  std::string value = (tokens.size() == 1) ? tokens[0] : join(tokens, COLON);
  value_ = std::move(value);
}

Header::Header(Header &&other)
    : key_(std::move(other.key_)), value_(std::move(other.value_)),
      valid_(other.valid_) {}

Header &Header::operator=(Header &&other) {
  key_ = std::move(other.key_);
  value_ = std::move(other.value_);
  valid_ = other.valid_;
  return *this;
}

auto Header::valid() const -> bool { return valid_; }

auto Header::key() const -> std::string { return key_; }

auto Header::value() const -> std::string { return value_; }

void Header::setValue(const std::string &new_value) noexcept {
  value_ = new_value;
}

auto Header::serialize() const -> std::string {
  return key_ + COLON + value_ + CRLF;
}

auto operator<<(std::ostream &os, const Header &header) -> std::ostream & {
  os << "HTTP Header contains:" << std::endl;
  os << "Key: " << header.key() << std::endl;
  os << "Value: " << header.value() << std::endl;
  os << "IsValid: " << ((header.valid()) ? "True" : "False") << std::endl;
  os << "Serialize to: " << header.serialize();
  return os;
}

}  // namespace http

}  // namespace falconlink
