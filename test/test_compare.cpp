/**
 * @file  test_compare.cpp
 * @author  Sandeep <deep@tensorfield.ag>
 * @version 1.0
 *
 * @section LICENSE
 *
 * MIT License <http://opensource.org/licenses/MIT>
 *
 * @section DESCRIPTION
 *
 * Test queue implementation by comparing state against different implementations
 * of fixed size queue.
 *
 * https://github.com/d-e-e-p/fixed-size-string-buffer
 * Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
 *
 */

#include <iostream>
#include <sstream>
#include <regex>
#include <stdlib.h>     /* srand, rand */
#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include "fixed_size_string_buffer.h"
#include "fixed_char_size_queue.h"
#include "fixed_elem_size_queue.h"


TEST(Compare, WithFixedCharSizeQueue) {

  constexpr size_t ring_buffer_size_chars = 100;
  auto q_ssb = FixedSizeStringBuffer<ring_buffer_size_chars>();
  auto q_csq = FixedCharSizeQueue(ring_buffer_size_chars);

  constexpr size_t seed = 420;
  srand(seed);

  int num_iter = 100; 
  for (auto i=0; i<num_iter; i++) {
    auto num = rand() % ring_buffer_size_chars;
    auto char_num = char(num);
    auto str_test = std::string(num, char_num);
    q_ssb.push(str_test);
    q_csq.push(str_test);
  }

  while( !q_ssb.empty()) {
    EXPECT_THAT(q_ssb.pop(), testing::StrEq(q_csq.pop()));
  }


}

TEST(Compare, WithFixedStrSizeQueue) {

  constexpr size_t ring_buffer_size_chars = 100;
  constexpr size_t strlen = 10;

  auto q_ssb = FixedSizeStringBuffer<ring_buffer_size_chars>();
  auto q_esq = FixedElemSizeQueue<std::string>(ring_buffer_size_chars/strlen);

  constexpr size_t seed = 420;
  srand(seed);



  int num_iter = 1000;
  for (auto i=0; i < num_iter; ++i) {
    auto num = rand() % 24;
    auto char_num = char('a' + num );
    auto str_test = std::string(strlen, char_num);
    q_ssb.push(str_test);
    q_esq.push(str_test);
  }


  while( !q_ssb.empty()) {
    EXPECT_THAT(q_ssb.pop(), testing::StrEq(q_esq.pop()));
  }


}

TEST(Compare, WithUnlimitedQueue) {

  constexpr size_t ring_buffer_size_chars = 1000;
  auto q_fsb = FixedSizeStringBuffer<ring_buffer_size_chars>();
  auto q_ulq = std::deque<std::string>();

  constexpr size_t seed = 420;
  srand(seed);

  int num_iter = 100; 
  for (auto i=0; i<num_iter; i++) {
    auto num = rand() % ring_buffer_size_chars / 10;
    std::stringstream obj;
    obj << (num % 10); 
    char char_num = obj.str()[0];
    auto str_test = std::string(num, char_num);
    q_fsb.push(str_test);
    q_ulq.push_back(str_test);
  }

  size_t qdiff = q_ulq.size() - q_fsb.size();
  for (auto i=0 ; i < q_fsb.size() ; i++) {
    EXPECT_THAT(q_ulq[i+qdiff], testing::StrEq(q_fsb[i]));
  }


}

