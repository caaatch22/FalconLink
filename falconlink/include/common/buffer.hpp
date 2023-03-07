#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "common/macros.hpp"

namespace falconlink {

static constexpr size_t INITIAL_BUFFER_CAPACITY = 1024;

/**
 * This Buffer abstracts an underlying dynamic char array
 * that allows pushing in byte data from two ends
 * NOT thread-safe
 * */
class Buffer {
 public:
  explicit Buffer(size_t initial_capacity = INITIAL_BUFFER_CAPACITY);

  ~Buffer() = default;

  Buffer(const Buffer &other) = default;

  Buffer &operator=(const Buffer &other) = default;

  NON_MOVEABLE(Buffer);

  void append(const unsigned char *new_char_data, size_t data_size);

  void append(const std::string &new_str_data);

  void append(std::vector<unsigned char> &&other_buffer);

  void appendFront(const unsigned char *new_char_data, size_t data_size);

  void appendFront(const std::string &new_str_data);

  auto findAndPopTill(const std::string &target) -> std::optional<std::string>;

  size_t size() const;

  size_t capacity() const;

  const unsigned char * data();

  std::string_view toStringView() const;

  void clear();

 private:
  std::vector<unsigned char> buf_;
};

}  // namespace falconlink
