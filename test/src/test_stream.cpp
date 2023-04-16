/**
 * @file  test_stream.cpp
 * @author  Sandeep <deep@tensorfield.ag>
 * @version 1.0
 *
 * @section LICENSE
 *
 * MIT License <http://opensource.org/licenses/MIT>
 *
 * @section DESCRIPTION
 *
 * Test ring buffer error and description messages
 *
 * https://github.com/d-e-e-p/fixed-size-string-buffer
 * Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
 *
 */
#include <iostream>
#include <sstream>
#include <regex>
#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include "fssb/fixed_size_string_buffer.h"
#include "fssb/fixed_char_size_queue.h"

class StreamRedirect 
{
public:
    std::stringstream ss;
    explicit StreamRedirect(std::ostream& new_stream) : old_stream{new_stream} 
    {
        old_buf = old_stream.rdbuf();
        old_stream.rdbuf(ss.rdbuf());
    }
    ~StreamRedirect() {
        old_stream.rdbuf(old_buf);
    }
private:
    std::ostream& old_stream;
    std::streambuf* old_buf;
};

TEST(StreamTest, FixedSizeStringBuffer) {

  // override cout and cerr
  StreamRedirect serr(std::cerr);

  constexpr size_t ring_buffer_char_size = 10;
  auto buffer = fssb::FixedSizeStringBuffer<ring_buffer_char_size>();
  std::string output;

  // test0 : pop on empty
  std::string msg0;

  serr.ss.str("");

  msg0 = buffer.pop();

  output = serr.ss.str();
  EXPECT_THAT(output, testing::StartsWith("buffer is empty"));
  EXPECT_THAT(msg0,   testing::StartsWith("buffer is empty"));

  serr.ss.str("");

  msg0 = buffer[0];

  output = serr.ss.str();
  EXPECT_THAT(output, testing::StartsWith("buffer is empty"));
  EXPECT_THAT(msg0,   testing::StartsWith("buffer is empty"));

  // test1 : push too big
  serr.ss.str("");
  size_t len1 = ring_buffer_char_size + 1;
  auto str1 = std::string(len1, 'a');

  buffer.push(str1);

  output = serr.ss.str();
  std::string msg = "string length : " + std::to_string(str1.length()) +
                      " > max size " + std::to_string(ring_buffer_char_size);
  EXPECT_THAT(output, testing::StartsWith(msg));

  // test2 : access invalid element
  serr.ss.str("");
  buffer.clear();
  size_t len2 = ring_buffer_char_size - 1;
  auto str2 = std::string(len2, 'a');

  buffer.push(str2);
  size_t pos = 2;
  std::string outmsg = buffer[pos];

  output = serr.ss.str();
  msg = "no element at index " + std::to_string(pos) +
            " : max index is " + std::to_string(0);
  EXPECT_THAT(output, testing::StartsWith(msg));

  // test3 : access element
  serr.ss.str("");
  buffer.clear();
  size_t len3 = ring_buffer_char_size - 1;
  auto str3 = std::string(len3, 'a');

  buffer.push(str3);
  outmsg = buffer[-1];

  output = serr.ss.str();
  msg = "no element at index";
  EXPECT_THAT(output, testing::StartsWith(msg));
  
}


TEST(StreamTest, FixedCharSizeQueue) {

  // override cout and cerr
  StreamRedirect serr(std::cerr);

  constexpr size_t ring_buffer_char_size = 10;
  auto buffer = fssb::FixedCharSizeQueue(ring_buffer_char_size);
  std::string output;

  // test1 : push too big
  serr.ss.str("");
  size_t len1 = ring_buffer_char_size + 1;
  auto str1 = std::string(len1, 'a');

  buffer.push(str1);

  output = serr.ss.str();
  std::string msg = "string length : " + std::to_string(str1.length()) +
                      " > max size " + std::to_string(ring_buffer_char_size);
  EXPECT_THAT(output, testing::StartsWith(msg));


}
