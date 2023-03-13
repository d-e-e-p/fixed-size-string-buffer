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



#include <deque>
#include <iostream>
#include <iomanip>
#include <vector>

// #include "FixedSizeStringBuffer.hpp"

namespace buffer {
//
// Constructor
//
FixedSizeStringBuffer::FixedSizeStringBuffer(char chars[], size_t max_size)
    : chars_(chars),  max_size_(max_size)
{
  clear();
};

/*
FixedSizeStringBuffer::FixedSizeStringBuffer(std::array<char> chars)
    : chars_(chars.data()), max_size_(chars.max_size())
{
  clear();
};
*/

//
// Capacity
//
bool FixedSizeStringBuffer::empty() const { return ptr.empty(); }
size_t FixedSizeStringBuffer::size() const { return ptr.size(); }
size_t FixedSizeStringBuffer::free_space() const { return free_space_; }

//
// Modifiers
//
void FixedSizeStringBuffer::set_debug(bool debug)
{
  debug_ = debug;
  std::cout << " debug set to " << std::boolalpha << debug << "\n";
}
void FixedSizeStringBuffer::clear()
{
  ptr.clear();
  strsizes_.clear();
  back_ = 0;
  free_space_ = max_size_;
}

void FixedSizeStringBuffer::push_simple(std::string_view str)
{
  // can str fit in chars?
  if (str.length() > max_size_) {
    const std::string msg = "string length : " + std::to_string(str.length()) +
                      " > max size " + std::to_string(max_size_);
    std::cerr << msg << "\n";
    return;
  }
  clear_space_for_str(back_, str.length());
  // breaking string into segments and using std::copy isn't much faster..so
  size_t i = back_;
  for (char ch : str) {
    chars_[i] = ch;
    incr(i);
  }
  ptr.push_back(back_);
  back_ = i;
}

void FixedSizeStringBuffer::push(std::string_view str)
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
    ptr.pop_front();
  }

  size_t start = back_;
  size_t end = start + strlen;
  //auto c_copy_start = chars_.begin() + static_cast<long>(start);
  char* c_copy_start = &chars_[start];

  if (end < max_size_) {
    // case1: str is in one segment
    // |   [start]-->[end]    |
    std::copy(str.begin(), str.end(), c_copy_start);
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
    std::copy(str1.begin(), str1.end(), c_copy_start);
    //std::copy(str2.begin(), str2.end(), chars_.begin());
    std::copy(str2.begin(), str2.end(), chars_);
  }
  ptr.push_back(start);
  strsizes_.push_back(strlen);
  free_space_ -= strlen;
}

std::string FixedSizeStringBuffer::pop()
{
  std::string str = at(0);
  ptr.erase(ptr.begin());
  return str;
}

//
// Element access
//
std::string FixedSizeStringBuffer::front() const { return at(0); }
std::string FixedSizeStringBuffer::back() const { return at(ptr.size() - 1); }
std::string FixedSizeStringBuffer::operator[](size_t pos) const { return at(pos); }

std::string FixedSizeStringBuffer::at(size_t pos) const
{
  if (ptr.empty()) {
    std::string msg = "buffer is empty..";
    std::cerr << msg << "\n";
    return msg;
  }
  // reject out of bound requests
  if (pos >= ptr.size()) {
    std::string msg = "no element at index " + std::to_string(pos) +
                      " : max index is " + std::to_string(ptr.size() - 1);
    std::cerr << msg << "\n";
    return msg;
  }
  // end of string marker is either start of next string
  // or _back posize_ter for last string in queue
  size_t start = ptr[pos];
  size_t end = (pos + 1 == ptr.size()) ? back_ : ptr[pos + 1];

  // str is in one segment  |   [start]-->[end]    |
  // str wraps around ring  |-->[end]   [start]--->|
  std::string str;
  if (end > start) { str.append(&chars_[start], &chars_[end]); }
  else {
    str.append(&chars_[start], &chars_[max_size_]);
    str.append(&chars_[0], &chars_[end]);
  }
  return str;
}

//
// Helpers
//
void FixedSizeStringBuffer::incr(size_t &i)
{
  i++;
  if (i == max_size_) { i = 0; } // wrap around on max
}
// clear the range of next entry
void FixedSizeStringBuffer::clear_space_for_str(size_t from, size_t strlen)
{
  if (ptr.empty()) return;
  if (debug_) {
    std::cout << "clearing range from=" << from << " strlen=" <<  strlen  << "\n";
    for (size_t k = 0; k < ptr.size(); k++) {
      std::cout << "B  ptr[" << k << "] = " << ptr[k] << "\n";
    }
    std::cout << "B    back = " << back_ << "\n";
  }

  size_t start = from;
  size_t end = start + strlen;
  if (end < max_size_) {

    auto to_be_removed = std::remove_if(ptr.begin(), ptr.end(), [start,end] (auto pos){
      return (pos >= start && pos < end);
    });
    ptr.erase(to_be_removed, ptr.end());

  } else {

    size_t tail_segment = strlen - (max_size_ - start);
    size_t max_size = max_size_;
    auto to_be_removed = std::remove_if(ptr.begin(), ptr.end(), [start, tail_segment, max_size] (auto pos){
      return (pos >= start && pos < max_size) ||  (pos >= 0 && pos < tail_segment);
    });
    ptr.erase(to_be_removed, ptr.end());

  }

  if (debug_) {
    for (size_t k = 0; k < ptr.size(); k++) {
      std::cout << "A  ptr[" << k << "] = " << ptr[k] << "\n";
    }
  }
}

// output char buffer with markers for each string
void FixedSizeStringBuffer::dump(std::ostream &os = std::cout) const
{
  for (size_t i = 0; i < max_size_; i++) {
    os << " chars[" << std::setw(2) << i << "] = " << chars_[i];
    for (size_t k = 0; k < ptr.size(); k++) {
      if (ptr[k] == i) os << " <-- ptr[" << std::setw(2) << k << "]";
    }
    if (! empty()) {
      if (ptr[0] == i) os << " <-- front ";
    }
    if (back_ == i) os << " <-- back ";
    os << "\n";
  }

  os << "\n";
  for (size_t k = 0; k < ptr.size(); k++) {
    os << "   ptr[" << std::setw(2) << k << "] = " << ptr[k] << " str = " << at(k) << "\n";
  }
}

std::ostream &operator<<(std::ostream &os, FixedSizeStringBuffer &rb)
{
  rb.dump(os);
  return os;
};

};  // namespace buffer


