#include <iostream>
#include <sstream>
#include <regex>
#include <stdlib.h>     /* srand, rand */
#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include "fixed_size_string_buffer.h"
#include "fixed_size_queue.h"


TEST(Compare, WithFixedQueue) {

  constexpr size_t ring_buffer_char_size = 100;
  auto fsb = FixedSizeStringBuffer<ring_buffer_char_size>();
  auto rfq = FixedQueue(ring_buffer_char_size);

  constexpr size_t seed = 420;
  srand(seed);

  int num_iter = 100; 
  for (auto i=0; i<num_iter; i++) {
    auto num = rand() % ring_buffer_char_size;
    auto char_num = char(num % 10);
    auto str_test = std::string(num, char_num);
    fsb.push(str_test);
    rfq.push(str_test);
  }

  while( !rfq.empty()) {
    EXPECT_THAT(rfq.pop(), testing::StrEq(fsb.pop()));
  }


}

TEST(Compare, WithUnlimitedQueue) {

  constexpr size_t ring_buffer_char_size = 1000;
  auto fsb = FixedSizeStringBuffer<ring_buffer_char_size>();
  std::deque<std::string> ulq;

  constexpr size_t seed = 420;
  srand(seed);

  int num_iter = 100; 
  for (auto i=0; i<num_iter; i++) {
    auto num = rand() % ring_buffer_char_size / 10;
    std::stringstream obj;
    obj << (num % 10); 
    char char_num = obj.str()[0];
    auto str_test = std::string(num, char_num);
    fsb.push(str_test);
    ulq.push_back(str_test);
  }

  size_t qdiff = ulq.size() - fsb.size();
  for (auto i=0 ; i < fsb.size() ; i++) {
    EXPECT_THAT(ulq[i+qdiff], testing::StrEq(fsb[i]));
  }


}

