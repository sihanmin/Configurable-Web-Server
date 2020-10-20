#include "gtest/gtest.h"
#include "config_parser.h"
#include <fstream>
#include <streambuf>

class NginxConfigParserTestF : public ::testing::Test{
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};

TEST(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("test_config/example_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestF, AlmostEmptyConfig) { //config file with empty statements like {}
  bool success = parser.Parse("test_config/example_config_2", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestF, EmptyConfig) { //empty config file
  bool success = parser.Parse("test_config/example_config_3", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestF, NoSemiColon) { //config file with statement without token
  bool success = parser.Parse("test_config/example_config_4", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTestF, UnmatchedQuotes) {
  bool success = parser.Parse("test_config/example_config_5", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTestF, UnmatchedCurlyBraces1) {
  bool failure = parser.Parse("test_config/example_config_6", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F(NginxConfigParserTestF, UnmatchedCurlyBraces2) {
  bool failure = parser.Parse("test_config/example_config_8", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F(NginxConfigParserTestF, ValidPortNumber) {
  bool success = parser.Parse("config", &out_config);
  int portnum = out_config.GetPortNumber();
  EXPECT_TRUE(success);
  EXPECT_EQ(portnum, 8080);
}

TEST_F(NginxConfigParserTestF, InvalidPortNumber) {
  bool success = parser.Parse("test_config/invalid_portnum_config", &out_config);
  int portnum = out_config.GetPortNumber();
  EXPECT_TRUE(success);
  EXPECT_EQ(portnum, -1);
}

TEST_F(NginxConfigParserTestF, NonExistingFile) {
  bool failure = parser.Parse("test_config/non_existing_config", &out_config);
  EXPECT_FALSE(failure);
}

TEST_F(NginxConfigParserTestF, ToString) {
  bool success = parser.Parse("test_config/example_config_7", &out_config);
  std::string serilaized_config = out_config.ToString(2);
  std::ifstream t("test_config/example_config_7");
  std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  EXPECT_EQ(str, serilaized_config);
}