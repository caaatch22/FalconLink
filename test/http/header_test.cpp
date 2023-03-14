#include "http/header.hpp"

#include "gtest/gtest.h"
#include "http/http_utils.hpp"

namespace falconlink {

namespace http {

TEST(HeaderTest, ConstructTest) {
  std::string k = "http_version";
  std::string v1 = "1.1";
  std::string v2 = "1.2";
  Header h1(k, v1);
  EXPECT_TRUE(h1.valid());
  EXPECT_EQ(h1.key(), k);
  EXPECT_EQ(h1.value(), v1);

  h1.setValue(v2);
  EXPECT_EQ(h1.value(), v2);
}

TEST(HeaderTest, SerializeTest) {
  std::string valid_header_line = "Host:FalconLink";
  std::string invalid_header_line = "Hello& No Way!";
  Header h1(valid_header_line);
  Header h2(invalid_header_line);

  EXPECT_TRUE(h1.valid());
  EXPECT_EQ(h1.key(), "Host");
  EXPECT_EQ(h1.value(), "FalconLink");

  // no colon in this header line, is not valid
  EXPECT_FALSE(h2.valid());

  std::string serialized = h1.serialize();
  EXPECT_EQ(serialized, (valid_header_line + CRLF));
}

}  // namespace http

}  // namespace falconlink
