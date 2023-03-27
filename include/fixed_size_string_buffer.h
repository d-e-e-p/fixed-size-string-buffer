/**
 * @file
 * @author  Sandeep <deep@tensorfield.ag>
 * @version 1.0
 *
 * @section LICENSE
 *
 * MIT License <http://opensource.org/licenses/MIT>
 *
 * @section DESCRIPTION
 *
 * A compile-time allocated ring buffer for std::string messages
 * https://github.com/d-e-e-p/fixed-size-string-buffer
 * Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
 * 
 */

#pragma once
#pragma clang diagnostic ignored "-Wnarrowing"

#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <map>
#include <deque>
#include <iomanip>
#include <codecvt>
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
                                 
  // used for pretty print of buffer
  struct SlotState {
    std::vector<bool> bopen;
    std::vector<bool> bword;
    std::vector<bool> bclos;
  } ;
  SlotState mark_open_close_slots();
  void print_box_line(std::ostream &os, const SlotState& slot, bool is_top) const;
  void print_char_line(std::ostream &os, const SlotState& slot) const;

 public:
  /**
   * Constructor that creates a string buffer of fixed character size
   *
   * @param SPACE is size of char std:array to allocate at compile-time
   */
  FixedSizeStringBuffer<SPACE>()
      : max_space_(SPACE) 
  { 
    clear(); 
  }

  //
  // Capacity
  //
  bool empty() const  { return ptr_.empty(); }
  bool full() const  { return (free_space_ == 0); }
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

  std::string pop()
  {
    if (empty()) {
      std::string msg = "buffer is empty\n";
      std::cerr << msg;
      return msg;
    }
    std::string str = front();
    free_space_ += ptr_.front().len;
    ptr_.pop_front();
    if (empty()) {
      back_ = 0;
    }
    return str;
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
  void dump_long_str(std::ostream &os = std::cout) const;
  void dump_short_str(std::ostream &os = std::cout);
  void dump(std::ostream &os = std::cout) 
  {
    constexpr size_t threshold = 40;
    if(max_space_ > threshold) {
      dump_long_str(os);
    } else {
      dump_short_str(os);
    }
  }

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

// NOLINTBEGIN
namespace {
// from https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
std::string ws2s(const std::wstring& wstr) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.to_bytes(wstr);
}
}
// NOLINTEND

template <size_t SPACE>
typename FixedSizeStringBuffer<SPACE>::SlotState FixedSizeStringBuffer<SPACE>::mark_open_close_slots() 
{
  // define vector (...or use map for longer strings?)
  SlotState slot;
  slot.bopen = std::vector<bool>(SPACE, false);
  slot.bword = std::vector<bool>(SPACE, false);
  slot.bclos = std::vector<bool>(SPACE, false);

  // loop thru string pointer and mark every open close slot
  // last =  end char marker moved left-by-1 to point to actual last char
  for (size_t k = 0; k < ptr_.size(); k++) {
    size_t start = ptr_[k].front;
    size_t end = (k+1 == ptr_.size()) ? back_ : ptr_[k+1].front;
    size_t last = (end == 0) ? max_space_ - 1 : end - 1;

    slot.bopen[start] = true;
    slot.bclos[last] = true;

    if (end > start) {
      for (size_t i = start; i < end; i++) {
        slot.bword[i] = true;
      }
    } else {
      for (size_t i = start; i < max_space_; i++) {
        slot.bword[i] = true;
      }
      for (size_t i = 0; i < end; i++) {
        slot.bword[i] = true;
      }
    }
  }

  return slot;
}

template <size_t SPACE>
void FixedSizeStringBuffer<SPACE>::print_box_line(std::ostream &os, const SlotState& slot, bool is_top) const
{
  enum class CT {left, open, close, dash, space, right};
  typedef std::map<CT,wchar_t> box_t;

  box_t box_top = {
      {CT::left,  L'⎧'},
      {CT::open,  L'╭'},
      {CT::close, L'╮'},
      {CT::dash,  L'─'},
      {CT::space, L' '},
      {CT::right, L'⎫'},
   };

  box_t box_bot = {
      {CT::left,  L'⎩'},
      {CT::open,  L'╰'},
      {CT::close, L'╯'},
      {CT::dash,  L'─'},
      {CT::space, L' '},
      {CT::right, L'⎭'},
   };


  box_t box = is_top ? box_top : box_bot;

  // insert space on left
  // NOLINTNEXTLINE (-Wold-style-cast)
  std::wstring wstr = (std::wstring) L"           " + box[CT::left] + L' ';
  os << ws2s(wstr);
  // characters
  for (size_t i = 0; i < max_space_; i++) {
    wchar_t copen  = box[CT::space];
    wchar_t cchar  = box[CT::space];
    wchar_t cclos = box[CT::space];

    if (slot.bword[i]) {
      copen  = box[CT::dash];
      cchar  = box[CT::dash];
      cclos = box[CT::dash];
    }

    if (slot.bopen[i]) {
      copen  = box[CT::open];
    }

    if (slot.bclos[i]) {
      cclos  = box[CT::close];
    }

    std::wstring wline{copen, cchar, cclos};
    os << ws2s(wline);
  }
  std::wstring wline{box[CT::right]};
  os << ws2s(wline) << "\n";

}

template <size_t SPACE>
void FixedSizeStringBuffer<SPACE>::print_char_line(std::ostream &os, const SlotState& slot) const
{

  os << " buf[" << std::setw(2) << max_space_ << "] = ⎨ ";
  for (size_t i = 0; i < max_space_; i++) {
    // 3 slots for each char: [close][open][letter]
    wchar_t copen = slot.bopen[i] ? L'│' : L' ';
    wchar_t cchar = (chars_[i] == '\0') ? L'•' : chars_[i];
    wchar_t cclos = slot.bclos[i] ? L'│' : L' ';
    // mark first element and end of last element in queue
    if (! empty()) {
      if (i == ptr_[0].front) {
        copen = L'┤';
      }
      size_t back_left = (back_ == 0) ? max_space_ - 1 : back_ - 1;
      if (i == back_left) {
        cclos = L'├';
      }
    }
    std::wstring wline{copen, cchar, cclos};
    os << ws2s(wline);
  }
  os << "⎬ \n";

}

template <size_t SPACE>
void FixedSizeStringBuffer<SPACE>::dump_short_str(std::ostream &os) 
{
    
  SlotState slot = mark_open_close_slots();

  print_box_line(os, slot, true);
  print_char_line(os, slot);
  print_box_line(os, slot, false);

}


template <size_t SPACE>
void FixedSizeStringBuffer<SPACE>::dump_long_str(std::ostream &os) const
{
  for (size_t i = 0; i < max_space_; i++) {
    os << "  c[" << std::setw(2) << i << "] = " << chars_[i];
    for (size_t k = 0; k < ptr_.size(); k++) {
      if (ptr_[k].front == i) { 
        os << " <-- str[" << std::setw(2) << k << "] = " << at(k) ; 
      }
    }
    if (back_ == i) { os << " <-- back "; }
    os << "\n";
  }
  os << "\n";
}

template <size_t SPACE>
std::ostream &operator<<(std::ostream &os, FixedSizeStringBuffer<SPACE> &rb)
{
  rb.dump(os);
  return os;
}
