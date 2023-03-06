#pragma once
#include <string>

namespace falconlink {

class Buffer {
 public:
  Buffer() = default;
  ~Buffer() = default;

  void append(const char* _str, int _size);
  ssize_t size() const;
  const char* c_str();
  void clear();
  void getline();
  void setBuf(const char*);

 private:
  std::string buf_;
};

} // namespace falconlink


