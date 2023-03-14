#include "common/buffer.hpp"

#include <cstring>

#include "gtest/gtest.h"

namespace falconlink {

TEST(BufferTest, BasicTest) {
  Buffer buf(INITIAL_BUFFER_CAPACITY);
  ASSERT_EQ(buf.size(), 0);
  ASSERT_EQ(buf.capacity(), INITIAL_BUFFER_CAPACITY);

  {
    const char msg1[1024] = "Greeting from beginning!";
    const char msg2[2014] = "Farewell from end~";
    buf.appendFront(msg1);
    buf.append(msg2);
    snprintf((char *)msg1 + strlen(msg1), strlen(msg2) + 1, "%s", msg2);
    EXPECT_EQ(std::strncmp((char *)buf.data(), msg1, std::strlen(msg1)), 0);
    buf.clear();
    EXPECT_EQ(buf.size(), 0);
  }

  {
    const std::string msg1 = "Greeting from beginning!";
    const std::string msg2 = "Farewell from end~";
    buf.appendFront(msg1);
    buf.append(msg2);
    const std::string together = msg1 + msg2;
    EXPECT_EQ(buf.toStringView(), together);
    buf.clear();
    EXPECT_EQ(buf.size(), 0);
  }

  {
    const std::string msg =
        "GET / HTTP/1.1\r\n"
        "Connection: Keep-Alive\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
        "Accept-Language: en-us\r\n"
        "\r\n";
    const std::string next_msg = "Something belongs to next message";
    buf.append(msg);
    buf.append(next_msg);
    auto op_str = buf.findAndPopTill("\r\n\r\n");
    EXPECT_TRUE(op_str.has_value());
    EXPECT_EQ(op_str.value(), msg);
    EXPECT_EQ(buf.toStringView(), next_msg);
  }
}

}  // namespace falconlink
