/*

 A compile-time allocated ring buffer for std::string messages
 https://github.com/d-e-e-p/fixed-size-string-buffer

 MIT License <http://opensource.org/licenses/MIT>
 Copyright (c) 2023 Sandeep <deep@tensorfield.ag>

 */
#pragma once

#include <deque>
#include <array>
#include <iostream>
#include <iomanip>


template <std::size_t SIZE>  
class FixedSizeStringBuffer {
 private:
  //  we could use std::vector<char> chars_; 
  //  to make char buffer dynamically resizable
  std::array<char,SIZE> chars_ = {};

  std::deque<size_t> ptr_;      // pointer to start of strings in chars_
  std::deque<size_t> strsizes_; // sizes of these strings
  const size_t max_size_ ;      // max num of *chars* in buffer
  size_t back_ = 0;             // pointer to last string in chars_
  size_t free_space_ = 0;       // free *char* space left in buffer
  bool debug_ = false;          // print diag messages if true

 public:
//
// Constructor
//
FixedSizeStringBuffer<std::size_t>()
    : max_size_(SIZE) 
{ 
      clear(); 
};



//
// Capacity
//
bool empty() const { return ptr_.empty(); }
size_t size() const { return ptr_.size(); }
size_t free_space() const { return free_space_; }

//
// Modifiers
//
void set_debug(bool debug)
{
  debug_ = debug;
  std::cout << " debug set to " << std::boolalpha << debug << "\n";
}
void clear()
{
  ptr_.clear();
  strsizes_.clear();
  back_ = 0;
  free_space_ = max_size_;
}

void push(std::string_view str)
{
  // can str fit in chars?
  size_t strlen = str.length();
  if (strlen > max_size_) {
    std::string msg = "string length : " + std::to_string(strlen) +
                      " > max size " + std::to_string(max_size_);
    std::cerr << msg << "\n";
    return;
  }

  // clear space for str
  while (free_space_ < strlen) {
    free_space_ += strsizes_.front();
    strsizes_.pop_front();
    ptr_.pop_front();
  }

  size_t start = back_;
  size_t end = start + strlen;

  if (end < max_size_) {
    // case1: str is in one segment
    // |   [start]-->[end]    |
    std::copy(str.begin(), str.end(), &chars_[start]);
    back_ = end;

  } else {
    //  case2: wrap around case
    //           segment = (max_size_ - start)
    // |-->[end]   [start]--->|
    size_t seg1 = max_size_ - start;
    size_t seg2 = strlen - seg1;
    back_ = seg2;

    std::string_view str1 = str.substr(0, seg1);
    std::string_view str2 = str.substr(seg1);
    std::copy(str1.begin(), str1.end(), &chars_[start]);
    std::copy(str2.begin(), str2.end(), &chars_[0]);
  }
  ptr_.push_back(start);
  strsizes_.push_back(strlen);
  free_space_ -= strlen;
}

std::string pop()
{
  std::string str = at(0);
  ptr_.erase(ptr_.begin());
  return str;
}

//
// Element access
//
std::string front() const { return at(0); }
std::string back() const { return at(ptr_.size() - 1); }
std::string operator[](size_t pos) const { return at(pos); }

std::string at(size_t pos) const
{
  // reject requests when empty
  if (ptr_.empty()) {
    std::string msg = "buffer is empty..";
    std::cerr << msg << "\n";
    return msg;
  }
  // reject out of bound requests
  if (pos >= ptr_.size()) {
    std::string msg = "no element at index " + std::to_string(pos) +
                      " : max index is " + std::to_string(ptr_.size() - 1);
    std::cerr << msg << "\n";
    return msg;
  }
  // end of string marker is either start of next string
  // or _back for last string in queue
  size_t start = ptr_[pos];
  size_t end = (pos + 1 == ptr_.size()) ? back_ : ptr_[pos + 1];

  // str is in one segment  |   [start]-->[end]    |
  // str wraps around ring  |-->[end]   [start]--->|
  std::string str;
  if (end > start) { 
    str.append(&chars_[start], &chars_[end]); 
  } else {
    str.append(&chars_[start], &chars_[max_size_]);
    str.append(&chars_[0], &chars_[end]);
  }
  return str;
}

//
// Helpers
//

// output char buffer with markers for each string
void dump(std::ostream &os = std::cout) const
{
  for (size_t i = 0; i < max_size_; i++) {
    os << " chars[" << std::setw(2) << i << "] = " << chars_[i];
    for (size_t k = 0; k < ptr_.size(); k++) {
      if (ptr_[k] == i) {os << " <-- ptr_[" << std::setw(2) << k << "]";}
    }
    if (! empty()) {
      if (ptr_[0] == i) {os << " <-- front ";}
    }
    if (back_ == i) {os << " <-- back ";}
    os << "\n";
  }

  os << "\n";
  for (size_t k = 0; k < ptr_.size(); k++) {
    os << "   ptr_[" << std::setw(2) << k << "] = " << ptr_[k] << " str = " << at(k) << "\n";
  }
}

};  // end class

template <std::size_t SIZE>  
std::ostream &operator<<(std::ostream &os, FixedSizeStringBuffer<SIZE> &rb) {
  rb.dump(os);
  return os;
}




