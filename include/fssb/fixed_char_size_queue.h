/**
 * @file  fixed_char_size_queue.h
 * @author  Sandeep <deep@tensorfield.ag>
 * @version 1.0
 *
 * @section LICENSE
 *
 * MIT License <http://opensource.org/licenses/MIT>
 *
 * @section DESCRIPTION
 *
 * A string queue with upper limit on number of characters:
 *    auto buf = FixedCharSizeQueue(max_size);
 *
 * https://github.com/d-e-e-p/fixed-size-string-buffer
 * Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
 *
 */

#pragma once

#include <queue>
#include <utility>
#include <iostream>

namespace {
struct Element {
  std::string str;
  size_t len;
  Element(std::string_view e_str, size_t e_len)
      : str(e_str), len(e_len)
  { }
};
}

class FixedCharSizeQueue : public std::queue<Element> {

private:
  size_t free_space_ = 0;
  //using std::queue<Element>::push_range; // disallow push_range

public:
  explicit FixedCharSizeQueue(size_t SIZE)
      : free_space_(SIZE)
  {
  }

  void push(const std::string& str);  
  std::string pop();

  // allow any std::string constructor as args to emplace
  template <typename... Args>
  void emplace(Args&&... args) {
    push(std::string(std::forward<Args>(args)...));
  }

  std::string front() {
    return std::queue<Element>::front().str;
  }
  std::string back() {
    return std::queue<Element>::back().str;
  }
};


inline void FixedCharSizeQueue::push(const std::string& str) 
{
  // clear space for str
  size_t strlen = str.length();
  while (free_space_ < strlen) {
    free_space_ += std::queue<Element>::front().len;
    std::queue<Element>::pop();
  }

  std::queue<Element>::emplace(str, strlen);
  free_space_ -= strlen;
}

inline std::string FixedCharSizeQueue::pop() 
{
  auto elem = std::queue<Element>::front();
  std::queue<Element>::pop(); 
  free_space_ += elem.len;
  return elem.str;
}

