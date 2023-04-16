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
  StreamRedirect sout(std::cout);
  StreamRedirect serr(std::cerr);

  constexpr size_t ring_buffer_char_size = 10;
  auto buffer = fssb::FixedSizeStringBuffer<ring_buffer_char_size>();
  std::string output;

  // test1 : pop on empty
  buffer.pop();

  output = serr.ss.str();
  EXPECT_THAT(output, testing::StartsWith("buffer is empty"));
  serr.ss.str("");

  int len1 = ring_buffer_char_size + 1;
  auto str1 = std::string(len1, 'a');


  
}


