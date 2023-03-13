/***********************************************************************
 * AUTHOR: deep@tensorfield.ag
 *   FILE:
 *   DATE: march 2023
 *  DESCR: fixed char size ring buffer to queue string messages
 ***********************************************************************/

#pragma once

#include <iostream>
#include <deque>

namespace buffer {

// static const size_t max_size_ = 10;
// std::vector<char> chars_(max_size_);
//

class FixedSizeStringBuffer {
 private:
  //  we could use std::vector<char> chars_; 
  //  to make char buffer dynamically resizable
  char* chars_;

  std::deque<size_t> ptr;       // start of strings in chars_
  std::deque<size_t> strsizes_; // sizes of these strings
  const size_t max_size_ ;      // max num of chars in buffer
  size_t back_ = 0;             // pointer to last string in chars_
  size_t free_space_ = 0;       // free char space left in buffer
  bool debug_ = false;          // print diag messages if true

 public:
  explicit FixedSizeStringBuffer(char chars[], size_t max_size);
  //explicit FixedSizeStringBuffer(std::array<char, size_t> chars);

  //
  // Capacity
  //
  bool empty() const;
  size_t size() const;          // number of *strings* in buffer
  size_t free_space() const;    // unused space in buffer (in chars)

  //
  // Modifiers
  //
  void set_debug(bool debug);
  void clear();
  void push(const std::string_view str);
  void push_simple(const std::string_view str);
  std::string pop();

  //
  // Element access
  //
  std::string front() const;
  std::string back() const;
  std::string operator[](size_t pos) const;
  std::string at(size_t pos) const;

  //
  // Helpers
  //
  void incr(size_t &i);
  void clear_space_for_str(size_t from, size_t strlen);
  void dump(std::ostream &os) const;

};  // end class

std::ostream &operator<<(std::ostream &os, FixedSizeStringBuffer &rb);

}  // namespace buffer
