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
  std::string  s1 = "🏴☠️";
  std::string  s2 = "\a\b\t\n\v\f\re";

  buffer.push(s1);
  ASSERT_EQ(buffer.free_space(), ring_buffer_char_size - s1.length());

  buffer.push(s2);

  ss.str(""); // clear
  ss << buffer;
  output = "\n" + ss.str();
  expect = R"(
            ⎧ ╭──────────────────────╮      ⎫
  buf[10] = ⎨ ┤␇  ␈  ␉  ␊  ␋  ␌  ␍  e├ �  � ⎬
            ⎩ ╰──────────────────────╯      ⎭
)";
  expect = std::regex_replace(expect, whitespace, "");
  output = std::regex_replace(output, whitespace, "");
  EXPECT_THAT(output, testing::StrEq(expect));

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
  buffer.dump(ss, ring_buffer_char_size - 1); // trigger dump_long_str
  output = ss.str();
  output = std::regex_replace(output, whitespace, "");

  EXPECT_THAT(output, testing::StartsWith(expect));

  ss.str(""); 
  buffer.dump(ss, ring_buffer_char_size + 1); // trigger dump_short_str
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

TEST(QueueTest, Swap) {

  constexpr size_t size = 30;

  auto buf1 = FixedSizeStringBuffer<size>();
  auto buf2 = FixedSizeStringBuffer<size>();

  const std::string str1 = "something\n";
  const std::string str2 = "completely different thing\n";

  buf1.swap(buf2);
  ASSERT_EQ(buf1.size(), 0);
  ASSERT_EQ(buf2.size(), 0);

  buf1.emplace(str1);
  buf2.emplace(str2);

  EXPECT_THAT(buf1[0], testing::StrEq(str1));
  EXPECT_THAT(buf2[0], testing::StrEq(str2));

  buf1.swap(buf2);

  EXPECT_THAT(buf1.pop(), testing::StrEq(str2));
  EXPECT_THAT(buf2.pop(), testing::StrEq(str1));

  ASSERT_EQ(buf1.free_space(), size);
  ASSERT_EQ(buf2.free_space(), size);
}

// test various methods of string creation...
// from https://cplusplus.com/reference/string/string/string/
TEST(QueueTest, Emplace) {

  constexpr size_t size = 100;
  auto buf = FixedSizeStringBuffer<size>();

  // constructors used in the same order as described above:
  std::string s0 ("Initial string", 8, 3);
  std::string s1 ("Another character sequence", 12);
  std::string s2 (10, 'x');
  std::string s3 (10, 42);      // 42 is the ASCII code for '*'

  buf.emplace("Initial string", 8, 3);
  buf.emplace("Another character sequence", 12);
  buf.emplace(10, 'x');
  buf.emplace(10, 42);      // 42 is the ASCII code for '*'
                                
  EXPECT_THAT(buf[0], testing::StrEq(s0));
  EXPECT_THAT(buf[1], testing::StrEq(s1));
  EXPECT_THAT(buf[2], testing::StrEq(s2));
  EXPECT_THAT(buf[3], testing::StrEq(s3));

}
