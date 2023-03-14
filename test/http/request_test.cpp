#include "http/request.hpp"

#include "gtest/gtest.h"
#include "http/header.hpp"
#include "http/http_utils.hpp"

namespace falconlink {

namespace http {

TEST(RequestTest, ParsingTest) {
  /* not end with /r/n */
  std::string request_0_str =
      "GET /hello.html HTTP/1.1\r\n"
      "User-Agent: Mozilla/4.0\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Connection: Keep-Alive\r\n";
  Request request_0{request_0_str};
  EXPECT_FALSE(request_0.valid());

  /* first line is not request line */
  std::string request_1_str =
      "User-Agent: Mozilla/4.0\r\n"
      "GET /hello.html HTTP/1.1\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Connection: Keep-Alive\r\n"
      "\r\n";
  Request request_1{request_1_str};
  EXPECT_FALSE(request_1.valid());

  /* method not supported */
  std::string request_2_str =
      "PUNCH /hello.html HTTP/1.1\r\n"
      "User-Agent: Mozilla/4.0\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Connection: Keep-Alive\r\n"
      "\r\n";
  Request request_2{request_2_str};
  EXPECT_FALSE(request_2.valid());

  /* version not supported */
  std::string request_3_str =
      "GET /hello.html HTTP/2.0\r\n"
      "User-Agent: Mozilla/4.0\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Connection: Keep-Alive\r\n"
      "\r\n";
  Request request_3{request_3_str};
  EXPECT_FALSE(request_3.valid());

  /* the bare minimum valid request */
  std::string request_4_str =
      "GET /hello.html HTTP/1.1\r\n"
      "\r\n";
  Request request_4{request_4_str};
  EXPECT_TRUE(request_4.valid());
  EXPECT_EQ(request_4.getMethod(), Method::GET);
  EXPECT_EQ(request_4.getVersion(), Version::HTTP_1_1);

  /* connection to close request */
  std::string request_5_str =
      "HEAD /hello.html HTTP/1.1\r\n"
      "User-Agent: Mozilla/4.0\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Connection: close\r\n"
      "\r\n";
  Request request_5{request_5_str};
  EXPECT_TRUE(request_5.valid());
  EXPECT_EQ(request_5.getMethod(), Method::HEAD);
  EXPECT_TRUE(request_5.shouldClose());

  /* connection to kepp alive request */
  std::string request_6_str =
      "GET /hello.html HTTP/1.1\r\n"
      "User-Agent: Mozilla/4.0\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Connection: Keep-Alive\r\n"
      "\r\n";
  Request request_6{request_6_str};
  EXPECT_FALSE(request_6.shouldClose());
}

}  // namespace http

}  // namespace falconlink
