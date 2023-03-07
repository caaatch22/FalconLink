#include "common/buffer.hpp"

namespace falconlink {

Buffer::Buffer(size_t initial_capacity) { buf_.reserve(initial_capacity); }

void Buffer::append(const unsigned char *new_char_data, size_t data_size) {
  buf_.insert(buf_.end(), new_char_data, new_char_data + data_size);
}

void Buffer::append(const std::string &new_str_data) {
  append(reinterpret_cast<const unsigned char *>(new_str_data.c_str()),
         new_str_data.size());
}

void Buffer::append(std::vector<unsigned char> &&other_buffer) {
  buf_.insert(buf_.end(), std::make_move_iterator(other_buffer.begin()),
              std::make_move_iterator(other_buffer.end()));
}

void Buffer::appendFront(const unsigned char *new_char_data, size_t data_size) {
  buf_.insert(buf_.begin(), new_char_data, new_char_data + data_size);
}

void Buffer::appendFront(const std::string &new_str_data) {
  appendFront(reinterpret_cast<const unsigned char *>(new_str_data.c_str()),
              new_str_data.size());
}

auto Buffer::findAndPopTill(const std::string &target)
    -> std::optional<std::string> {
  std::optional<std::string> res = std::nullopt;
  auto curr_content = toStringView();
  auto pos = curr_content.find(target);
  if (pos != std::string::npos) {
    res = curr_content.substr(0, pos + target.size());
    buf_.erase(buf_.begin(), buf_.begin() + pos + target.size());
  }
  return res;
}

size_t Buffer::size() const { return buf_.size(); }

size_t Buffer::capacity() const { return buf_.capacity(); }

const unsigned char *Buffer::data() { return buf_.data(); }

std::string_view Buffer::toStringView() const {
  return {reinterpret_cast<const char *>(buf_.data()), buf_.size()};
}

void Buffer::clear() { buf_.clear(); }

}  // namespace falconlink
