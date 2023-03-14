#include "http/response.hpp"

#include "gtest/gtest.h"
#include "http/header.hpp"
#include "http/http_utils.hpp"

namespace falconlink {

namespace http {

TEST(ResponseTest, BasicTest) {
  std::string status = "200 Success";
  Response response{RESPONSE_OK, false, std::string("nonexistent-file.txt")};
  auto headers = response.getHeaders();
  bool find = false;
  for (auto &h : headers) {
    if (h.key() == HEADER_CONTENT_LENGTH) {
      find = true;
    }
  }
  EXPECT_TRUE(find);
  std::string new_val = "1024";
  EXPECT_TRUE(response.changeHeader(HEADER_CONTENT_LENGTH, new_val));
  find = false;
  std::string value;
  headers = response.getHeaders();
  for (auto &h : headers) {
    if (h.key() == HEADER_CONTENT_LENGTH) {
      find = true;
      value = h.value();
    }
  }
  EXPECT_TRUE(find);
  EXPECT_EQ(value, new_val);
}

}  // namespace http

}  // namespace falconlink
