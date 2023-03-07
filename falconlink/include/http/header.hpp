#pragma once

#include <iostream>

namespace falconlink {

namespace http {

class Header {

 public:
  Header(const std::string &key, const std::string &value);
  Header(std::string &&key, std::string &&value);
  explicit Header(const std::string &line);  // deserialize method
  Header(const Header &other) = default;
  Header(Header &&other);
  Header &operator=(const Header &other) = default;
  Header &operator=(Header &&other);
  ~Header() = default;

  bool valid() const;
  std::string key() const;
  std::string value() const;
  void setValue(const std::string &new_value) noexcept;
  std::string serialize() const;

  friend auto operator<<(std::ostream &os, const Header &header) -> std::ostream &;

 private:
  std::string key_;
  std::string value_;
  bool valid_{true};

};


}  // namespace http


}  // namespace falconlink
