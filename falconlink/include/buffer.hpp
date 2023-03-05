#pragma once
#include <string>

namespace falconlink {

class Buffer {
 private:
  std::string buf_;

 public:
  Buffer();
  ~Buffer();

  void append(const char* _str, int _size);
  ssize_t size() const;
  const char* c_str();
  void clear();
  void getline();
  void setBuf(const char*);
};

} // namespace falconlink


