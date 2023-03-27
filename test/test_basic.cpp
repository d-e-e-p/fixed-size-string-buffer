#include <iostream>
#include <sstream>
#include <regex>
#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include "fixed_size_string_buffer.h"


TEST(QueueTest, Small) {

  constexpr size_t ring_buffer_char_size = 10;
  auto buffer = FixedSizeStringBuffer<ring_buffer_char_size>();

  int len1 = 3;
  int len2 = ring_buffer_char_size - len1;

  auto str1 = std::string(len1, 'a');
  auto str2 = std::string(len2, 'b');
  auto str3 = std::string(len1, 'c');

  // Push 2 strings onto the buffer
  buffer.push(str1);
  buffer.push(str2);

  // Check that the buffer is full
  ASSERT_TRUE(buffer.full());

  // Pop the oldest string from the buffer
  std::string oldest = buffer.pop();

  // Check that the popped string is the oldest string ("aaa")
  ASSERT_EQ(oldest, str1);

  // Push another string onto the buffer, same size as oldest
  buffer.push(str3);

  // Check that the buffer is still full
  ASSERT_TRUE(buffer.full());

  // Pop all the strings from the buffer and check their values
  ASSERT_EQ(buffer.pop(), str2);
  ASSERT_EQ(buffer.pop(), str3);

  // Check that the buffer is empty
  ASSERT_TRUE(buffer.empty());

  // test dumping#1
  buffer.clear();
  buffer.push(str1);
  buffer.push(str3);
  buffer.push(str2);
  std::stringstream ss;
  ss << buffer;
  std::string output = "\n" + ss.str();
  std::string expect = R"(
           ⎧ ────────╮╭───────╮╭───────────⎫
 buf[10] = ⎨  b  b  b├┤c  c  c││b  b  b  b ⎬
           ⎩ ────────╯╰───────╯╰───────────⎭
)";

  // strip space chars to avoid \r\n type problems
  std::regex whitespace("\\s+"); // space chars
  expect = std::regex_replace(expect, whitespace, "");
  output = std::regex_replace(output, whitespace, "");

  EXPECT_THAT(output, testing::StrEq(expect));

  // test dumping#2
  buffer.clear();
  buffer.push(str2);
  buffer.push(str2);
  buffer.push(str1);
  ss.str(""); // clear
  ss << buffer;
  output = "\n" + ss.str();
  expect = R"(
           ⎧ ───────────╮╭───────╮╭────────⎫
 buf[10] = ⎨  b  b  b  b││a  a  a├┤b  b  b ⎬
           ⎩ ───────────╯╰───────╯╰────────⎭
)";

  // strip space chars to avoid \r\n type problems
  // std::regex r("\\s+"); // space chars
  expect = std::regex_replace(expect, whitespace, "");
  output = std::regex_replace(output, whitespace, "");

  EXPECT_THAT(output, testing::StrEq(expect));

  // unicode test
  buffer.clear();
  std::string  str4 = "缓冲";
  buffer.push(str4);
  buffer.push(str4);
  buffer.push(str4);
  buffer.push(str4);

  EXPECT_THAT(buffer.front(), testing::StrEq(str4));
  EXPECT_THAT(buffer.back(),  testing::StrEq(str4));
  EXPECT_THAT(buffer.at(0),   testing::StrEq(str4));
  EXPECT_THAT(buffer[0],      testing::StrEq(str4));

  ASSERT_EQ(buffer.size(), 1);
  ASSERT_EQ(buffer.free_space(), ring_buffer_char_size - str4.length());

}

TEST(QueueTest, Warnings) {

  constexpr size_t ring_buffer_char_size = 42;
  auto buffer = FixedSizeStringBuffer<ring_buffer_char_size>();
  EXPECT_TRUE(buffer.empty()) << "buffer empty on start";
  // create string that can't fit and see what happens
  auto str1 = std::string(ring_buffer_char_size - 1, 'x');
  auto str2 = std::string(ring_buffer_char_size + 1, 'y');
  // from https://stackoverflow.com/questions/3803465/how-to-capture-stdout-stderr-with-googletest
  std::stringstream ss;
  std::streambuf *sbuf = std::cerr.rdbuf();
  std::cerr.rdbuf(ss.rdbuf());

  // expect stderr on second push
  buffer.push(str1);
  buffer.push(str2);

  std::string output = ss.str();
  std::cerr.rdbuf(sbuf);
  
  std::string expect = "string length : " 
    + std::to_string(ring_buffer_char_size + 1)
    + " > max size "
    + std::to_string(ring_buffer_char_size);

  EXPECT_THAT(output, testing::StartsWith(expect));
  ASSERT_FALSE(buffer.empty());

  EXPECT_THAT(buffer.pop(), testing::StartsWith("xxx"));

  ss.str(""); // clear
  std::cerr.rdbuf(ss.rdbuf());

  std::string strpop = buffer.pop();

  std::cerr.rdbuf(sbuf);
  output = ss.str();
  std::string msg = "buffer is empty";
  EXPECT_THAT(output, testing::StartsWith(msg));
  EXPECT_THAT(strpop, testing::StartsWith(msg));

  ss.str(""); 
  std::cerr.rdbuf(ss.rdbuf());

  std::string str_at = buffer.at(0);

  std::cerr.rdbuf(sbuf);
  output = ss.str();
  msg = "buffer is empty";
  EXPECT_THAT(output, testing::StartsWith(msg));
  EXPECT_THAT(str_at, testing::StartsWith(msg));

  auto str3 = std::string("test");
  buffer.push(str3);
  EXPECT_THAT(buffer.pop(), testing::StrEq(str3));
  ASSERT_TRUE(buffer.empty());

  buffer.push(str3);
  ss.str(""); 
  std::cerr.rdbuf(ss.rdbuf());

  str_at = buffer.at(1);

  std::cerr.rdbuf(sbuf);
  output = ss.str();
  msg = "no element at index ";

  EXPECT_THAT(output, testing::StartsWith(msg));
  EXPECT_THAT(str_at, testing::StartsWith(msg));

}

TEST(QueueTest, Large) {

  constexpr size_t ring_buffer_char_size = 100;
  auto buffer = FixedSizeStringBuffer<ring_buffer_char_size>();

  EXPECT_TRUE(buffer.empty()) << "buffer empty on start";
  int len1 = 30;
  int len2 = ring_buffer_char_size - len1;

  auto str1 = std::string(len1, 'a');
  auto str2 = std::string(len2, 'b');
  auto str3 = std::string(len1, 'c');

  // Push 2 strings onto the buffer
  buffer.push(str1);
  buffer.push(str2);

  std::stringstream ss;
  ss << buffer;
  std::string output = ss.str();
  std::string expect = R"(
c[0]=a<--str[0]=aaaaaaaaaaaaaaaaa
)";

  // strip space chars to avoid \r\n type problems
  std::regex whitespace("\\s+"); // space chars
  expect = std::regex_replace(expect, whitespace, "");
  output = std::regex_replace(output, whitespace, "");

  EXPECT_THAT(output, testing::StartsWith(expect));

  ss.str(""); // clear
  buffer.dump_long_str(ss);
  output = ss.str();
  output = std::regex_replace(output, whitespace, "");

  EXPECT_THAT(output, testing::StartsWith(expect));

  ss.str(""); 
  buffer.dump_short_str(ss);
  output = ss.str();
  expect = "⎧╭────────────────────────────────";
  output = std::regex_replace(output, whitespace, "");
  expect = std::regex_replace(expect, whitespace, "");

  EXPECT_THAT(output, testing::StartsWith(expect));

  std::streambuf *sbuf = std::cout.rdbuf();
  ss.str(""); // clear
  std::cout.rdbuf(ss.rdbuf());

  buffer.set_debug(true);

  std::cout.rdbuf(sbuf);
  output = ss.str();
  EXPECT_THAT(output, testing::StartsWith(" debug set to true"));



}
