/*

 A compile-time allocated ring buffer for std::string messages
 https://github.com/d-e-e-p/fixed-size-string-buffer

 MIT License <http://opensource.org/licenses/MIT>
 Copyright (c) 2023 Sandeep <deep@tensorfield.ag>

 */
#pragma once
#pragma clang diagnostic ignored "-Wnarrowing"

#include <array>
#include <deque>
#include <iomanip>
#include <iostream>

using std::size_t;

template <size_t SPACE>
class FixedSizeStringBuffer {
 private:

  std::array<char, SPACE> chars_ = {}; // main container for strings
                                      
  struct Pointer {
    size_t front;
    size_t len;
    Pointer(size_t p_front, size_t p_len)
        : front(p_front), len(p_len)
    { }
  };
  std::deque<Pointer> ptr_ = {}; // pointer to start/length of str in chars_
  size_t back_ = 0;              // pointer to end of last string in chars_
                                 //
  const size_t max_space_;        // max num of *chars* in buffer
  size_t free_space_ = 0;        // free *char* space left in buffer
                                 //
  bool debug_ = false;           // print diag messages if true

 public:
  //
  // Constructor
  //
  FixedSizeStringBuffer<SPACE>()
      : max_space_(SPACE) 
  { 
    clear(); 
  }

  //
  // Capacity
  //
  bool empty() const  { return ptr_.empty(); }
  size_t size() const { return ptr_.size(); }
  size_t free_space() const { return free_space_; }

  //
  // Modifiers
  //
  void set_debug(bool debug)
  {
    debug_ = debug;
    std::cout << " debug set to " << std::boolalpha << debug_ << "\n";
  }

  void clear()
  {
    chars_ = {};
    ptr_ = {};
    back_ = 0;
    free_space_ = max_space_;
  }

  void push(std::string_view str);  // see below

  void pop()
  {
    free_space_ += ptr_.front().len;
    ptr_.pop_front();
  }

  //
  // Element access
  //
  inline std::string front() const { return at(0); }
  inline std::string back() const { return at(ptr_.size() - 1); }
  inline std::string operator[](size_t pos) const { return at(pos); }
  inline std::string at(size_t pos) const; // see below

  //
  // Helpers
  //

  // output char buffer with markers for each string
  void dump(std::ostream &os = std::cout) const;

};  // end class

template <size_t SPACE>
void FixedSizeStringBuffer<SPACE>::push(std::string_view str)
{
  // can str fit in chars?
  size_t strlen = str.length();
  if (strlen > max_space_) {
    std::string msg = "string length : " + std::to_string(strlen) +
                      " > max size " + std::to_string(max_space_);
    std::cerr << msg << "\n";
    return;
  }

  // clear space for str .. repeat logic for pop or call pop?
  // that is the question.
  while (free_space_ < strlen) {
    free_space_ += ptr_.front().len;
    ptr_.pop_front();
  }

  size_t start = back_;
  size_t end = start + strlen;

  if (end < max_space_) {
    // case1: str is in one segment
    // |   [start]-->[end]    |
    std::copy(str.begin(), str.end(), &chars_[start]);
    back_ = end;

  } else {
    //  case2: wrap around case
    //             | seg1 = (max_space_ - start)
    // |-->[end]   [start]--->|
    size_t seg1 = max_space_ - start;
    size_t seg2 = strlen - seg1;
    back_ = seg2;

    std::copy(str.begin(), &str[seg1], &chars_[start]);
    std::copy(&str[seg1], str.end(), &chars_[0]);
  }
  //ptr_.push_back(Pointer{start, strlen});
  ptr_.emplace_back(start, strlen);
  free_space_ -= strlen;
}


template <size_t SPACE>
std::string FixedSizeStringBuffer<SPACE>::at(size_t pos) const
{
  // politely reject requests when empty
  if (ptr_.empty()) {
    std::string msg = "buffer is empty..";
    std::cerr << msg << "\n";
    return msg;
  }
  // reject out of bound requests with just a stern message
  if (pos >= ptr_.size()) {
    std::string msg = "no element at index " + std::to_string(pos) +
                      " : max index is " + std::to_string(ptr_.size() - 1);
    std::cerr << msg << "\n";
    return msg;
  }
  // end of string marker is either 
  //    - start of next string, or
  //    - _back for last string in queue
  size_t start = ptr_[pos].front;
  size_t end = (pos+1 == ptr_.size()) ? back_ : ptr_[pos+1].front;

  // str is in one segment  |   [start]-->[end]    |
  // str wraps around ring  |-->[end]   [start]--->|
  std::string str;
  if (end > start) { 
    str.append(&chars_[start], &chars_[end]); 
  } else {
    str.append(&chars_[start], &chars_[max_space_]);
    str.append(&chars_[0], &chars_[end]);
  }
  return str;
}


template <size_t SPACE>
void FixedSizeStringBuffer<SPACE>::dump(std::ostream &os) const
{
  for (size_t i = 0; i < max_space_; i++) {
    os << " chars[" << std::setw(2) << i << "] = " << chars_[i];
    for (size_t k = 0; k < ptr_.size(); k++) {
      if (ptr_[k].front == i) { os << " <-- ptr_[" << std::setw(2) << k << "]"; }
    }
    if (!empty()) {
      if (ptr_[0].front == i) { os << " <-- front "; }
    }
    if (back_ == i) { os << " <-- back "; }
    os << "\n";
  }

  os << "\n";
  for (size_t k = 0; k < ptr_.size(); k++) {
    os << "   ptr_[" << std::setw(2) << k << "] = " << ptr_[k].front
       << " str = " << at(k) << "\n";
  }
}

template <size_t SPACE>
std::ostream &operator<<(std::ostream &os, FixedSizeStringBuffer<SPACE> &rb)
{
  rb.dump(os);
  return os;
}
