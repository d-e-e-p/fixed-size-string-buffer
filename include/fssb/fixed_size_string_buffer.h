/*!
 * @file fixed_size_string_buffer.h
 * A compile-time allocated ring buffer for std::string messages
 *  @code{.cpp}
 *    auto rb = fssb::FixedSizeStringBuffer<10>();
 *  @endcode
 *
 * @author  Sandeep <deep@tensorfield.ag>
 * @version 1.0
 *
 * @section LICENSE
 *
 * MIT License <http://opensource.org/licenses/MIT>
 *
 * @section DESCRIPTION
 *
 * https://github.com/d-e-e-p/fixed-size-string-buffer
 * @copyright
 * Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
 * 
 */


#pragma once

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <unordered_map>
#include <deque>
#include <iomanip>
#include <codecvt>
#include <iostream>

#include <ctype.h>  // for isprint

namespace fssb {

using std::size_t;

/// @class FixedSizeStringBuffer
/// @brief A compile-time allocated ring buffer for std::string messages
///
/// Example usage:
/// @code{.cpp}
/// #include "fssb/fixed_size_string_buffer.h"
/// int main() {
///   auto rb = fssb::FixedSizeStringBuffer<10>();
///   rb.push("123");
///   rb.push("456");
///   rb.pop();
///   return 0;
/// }
/// @endcode
///
/// @tparam SPACE number of chars to allocate at compile-time
///
template <size_t SPACE>
class FixedSizeStringBuffer {

 private:
  // main storage for strings...search for plus1 for the reason for +1 
  std::array<char, SPACE+1> chars_ = {}; 
  // struct to store pointers to the start and length of a string in chars_.
  struct Pointer {
    size_t front;  // Position in char_ of start of string
    size_t len;    // String length cache
    Pointer(size_t p_front, size_t p_len) : front(p_front), len(p_len) {}
  };

  std::deque<Pointer> ptr_ = {};  // Pointer to start/length of string in chars_
  size_t back_ = 0;               // Pointer to end of last string in chars_
  size_t max_chars_;              // Maximum number of characters in buffer
  size_t free_chars_ = 0;         // Free character space left in buffer
  bool debug_ = false;            // Print diagnostic messages if true

 public:
  ///  @brief Constructor that creates a string buffer of fixed character size
  ///  template value (eg <10>) has to be a constexpr, ie known at compile-time
  FixedSizeStringBuffer<SPACE>()
      : max_chars_(SPACE) 
  { 
    clear(); 
  }

  // ***********************************************************************
  /// @name Capacity
  // ***********************************************************************
  //@{
  bool empty() const  { return ptr_.empty(); }        ///< @brief no strings in buffer
  bool full() const  { return (free_chars_ == 0); }   ///< @brief every char in buffer is occupied
  size_t size() const { return ptr_.size(); }         ///< @brief number of strings in buffer
  size_t free_space() const { return free_chars_; }   ///< @brief free *char* space in buffer
  //@} 

  // ***********************************************************************
  /// @name Modifiers
  // ***********************************************************************
  //@{

  /// @brief reset buffer to original empty state
  void clear()
  {
    chars_ = {};
    ptr_ = {};
    back_ = 0;
    free_chars_ = max_chars_;
  }

  /// @brief add elements to back of queue
  void push(std::string_view str);  
  /// @brief remove elements from front of queue
  std::string pop();
  /// @brief swap two queues of same character size
  void swap(FixedSizeStringBuffer<SPACE> &other);
  /// @brief push new entry into queue using any std::string constructor arguments
  template <typename... Args>
  void emplace(Args&&... args) {
    push(std::string(std::forward<Args>(args)...));
  }
  //@} 


  // ***********************************************************************
  /// @name Element access
  // ***********************************************************************
  //@{
  /// @brief oldest element still in buffer, rb[0]
  inline std::string front() const { return at(0); }            
  /// @brief newest element pushed into buffer, rb[rb.size() - 1]
  inline std::string back() const { return at(ptr_.size() - 1); }
  /// @brief access element using rb[1] notation
  inline std::string operator[](size_t pos) const { return at(pos); }
  /// @brief access element using rb.at(1) notation
  inline std::string at(size_t pos) const; // see below

  // ***********************************************************************
  /// @name Debug
  // ***********************************************************************
  //@{

  /// @brief turn on debug messages (TODO)
  void set_debug(bool debug)
  {
    debug_ = debug;
    std::cout << " debug set to " << std::boolalpha << debug_ << "\n";
  }

  /// @brief for debugging purposes, dump internal state of queue, 
  /// 
  /// both these variations show a represntation of the queue:
  /// @code
  ///   std::cout << rb;
  ///   rb.dump(std::cout);
  /// @endcode
  /// @code{.sh}
  ///           ⎧ ────────╮╭───────╮         ╭──⎫
  /// buf[10] = ⎨  a  z  y││D  o  g├ T  h  e ┤L ⎬
  ///           ⎩ ────────╯╰───────╯         ╰──⎭
  /// @endcode
  /// unprintable characters are marked with �, which is common when multi-char
  /// unicode, eg std::string("🏴☠️";),  is pushed into buffer.
  /// symbols are shows for unprintable chars, eg output for std::string("\t\n\v\f\r")
  /// @code{.sh}
  ///            ⎧ ╭─────────────╮               ⎫
  ///  buf[10] = ⎨ ┤␉  ␊  ␋  ␌  ␍├ •  •  •  •  • ⎬
  ///            ⎩ ╰─────────────╯               ⎭
  /// @endcode
  /// @param[in] threshold determines whether to use horizontal or vertical output
  /// @param[in] os output stream, eg std::cout
  void dump(std::ostream &os = std::cout, size_t threshold = 40) 
  {
    (max_chars_ > threshold) ? dump_long_str(os) : dump_short_str(os);
  }
  //@} 

 private:
  // used for pretty print of buffer
  struct SlotState {
    std::vector<bool> bopen;
    std::vector<bool> bword;
    std::vector<bool> bclos;
  } ;
  void dump_long_str(std::ostream &os = std::cout) const;
  void dump_short_str(std::ostream &os = std::cout);
  SlotState mark_open_close_slots();
  void print_box_line(std::ostream &os, const SlotState& slot, bool is_top) const;
  void print_char_line(std::ostream &os, const SlotState& slot) const;

};  // end class



///
/// push first creates space in buffer by silently removing oldest 
/// elements in queue until there is enough space. 
///
/// @param[in] str string to push into queue
/// @warning attemping to push a string larger than capacity fails 
/// just a with warning message, and queue is unchanged
/// 
template <size_t SPACE>
void FixedSizeStringBuffer<SPACE>::push(std::string_view str)
{
  // can str fit in chars?
  size_t strlen = str.length();
  if (strlen > max_chars_) {
    std::string msg = "string length : " + std::to_string(strlen) +
                      " > max size " + std::to_string(max_chars_);
    std::cerr << msg << "\n";
    return;
  }

  // clear space for str .. repeat logic for pop or call pop?
  // that is the question.
  while (free_chars_ < strlen) {
    free_chars_ += ptr_.front().len;
    ptr_.pop_front();
  }

  size_t start = back_;
  size_t end = start + strlen;

  if (end < max_chars_) {
    // case1: str is in one segment
    // |   [start]-->[end]    |
    std::copy(str.begin(), str.end(), &chars_[start]);
    back_ = end;

  } else {
    //  case2: wrap around case
    //             | seg1 = (max_chars_ - start)
    // |-->[end]   [start]--->|
    size_t seg1 = max_chars_ - start;
    size_t seg2 = strlen - seg1;
    back_ = seg2;

    //std::copy(&str[0], &str[seg1], &chars_[start]);
    //std::copy(&str[seg1], &str[strlen], &chars_[0]);

    str.copy(&chars_[start], seg1);
    str.copy(&chars_[0], seg2, seg1);

  }
  // use emplace object creation instead of ptr_.push_back(Pointer{start, strlen});
  ptr_.emplace_back(start, strlen);
  free_chars_ -= strlen;
}


///
/// pop removes and returns the oldest string in buffer. 
/// eg to remove all enteries in buffer
/// @code
///  while (! rb.empty()) {
///    rb.pop();
///  }
/// @endcode
///
/// @return string at front of buffer, rb.front() or rb[0]
/// @warning attemping to pop when the buffer is empty
/// issues a warning message to stderr and returns a string
/// with warning message. 
/// 
template <size_t SPACE>
std::string FixedSizeStringBuffer<SPACE>::pop()
{
  if (empty()) {
    std::string msg = "buffer is empty\n";
    std::cerr << msg;
    return msg;
  }
  std::string str = front();
  free_chars_ += ptr_.front().len;
  ptr_.pop_front();
  if (empty()) {
    back_ = 0;
  }
  return str;
}
///
/// at returns string value at position of buffer
///
/// @param[in] pos valid pos values are from 0 to rb.size() - 1
/// @return string at pos
/// @warning attemping to retrieve a value out of range
/// issues a warning message to stderr and also just returns 
/// a warning message instead of throwing an exception.
/// 

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
    str.append(&chars_[start], &chars_[max_chars_]); // <-- plus1 needed because of &chars_[max_chars_])
    str.append(&chars_[0], &chars_[end]);
  }
  return str;
}

///
/// swap can be used to exchange values between queues, eg
/// @code
///   constexpr size_t size = 30;
/// 
///   auto buf1 = FixedSizeStringBuffer<size>();
///   auto buf2 = FixedSizeStringBuffer<size>();
/// 
///   // push/pop operations on buf1/2
///   
///   buf1.swap(buf2);
///   
/// @endcode
/// @param[in] other another buffer of exactly the same char size
/// 
template <size_t SPACE>
void FixedSizeStringBuffer<SPACE>::swap(FixedSizeStringBuffer<SPACE> &other)
{
  using std::swap;
  swap(chars_, other.chars_);
  swap(ptr_, other.ptr_);
  swap(back_, other.back_);
  swap(max_chars_, other.max_chars_);
  swap(free_chars_, other.free_chars_);
  swap(debug_, other.debug_);
}

/*
 * output routines use a helper class
 *
 */

class fixed_size_string_buffer_utils {
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  // NOLINTBEGIN
  public:
  // from https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
  static std::string ws2s(const std::wstring& wstr) {
      using convert_typeX = std::codecvt_utf8<wchar_t>;
      std::wstring_convert<convert_typeX, wchar_t> converterX;
      return converterX.to_bytes(wstr);
  }
  //

  
  // https://stackoverflow.com/questions/39262323/print-a-string-variable-with-its-special-characters
  static wchar_t escaped(char const ch) {

      // from https://en.wikipedia.org/wiki/ASCII#Control_code_chart
    static const std::unordered_map<int, wchar_t> k_escapes =
      {
        {  0,   L'•' },  //     ^@      \0      Null
        {  1,   L'␁' },  //     ^A              Start of Heading
        {  2,   L'␂' },  //     ^B              Start of Text
        {  3,   L'␃' },  //     ^C              End of Text
        {  4,   L'␄' },  //     ^D              End of Transmission
        {  5,   L'␅' },  //     ^E              Enquiry
        {  6,   L'␆' },  //     ^F              Acknowledgement
        {  7,   L'␇' },  //     ^G      \a      Bell
        {  8,   L'␈' },  //     ^H      \b      Backspace
        {  9,   L'␉' },  //     ^I      \t      Horizontal Tab
        { 10,   L'␊' },  //     ^J      \n      Line Feed
        { 11,   L'␋' },  //     ^K      \v      Vertical Tab
        { 12,   L'␌' },  //     ^L      \f      Form Feed
        { 13,   L'␍' },  //     ^M      \r      Carriage Return
        { 14,   L'␎' },  //     ^N              Shift Out
        { 15,   L'␏' },  //     ^O              Shift In
        { 16,   L'␐' },  //     ^P              Data Link Escape
        { 17,   L'␑' },  //     ^Q              Device Control 1 (often XON)
        { 18,   L'␒' },  //     ^R              Device Control 2
        { 19,   L'␓' },  //     ^S              Device Control 3 (often XOFF)
        { 20,   L'␔' },  //     ^T              Device Control 4
        { 21,   L'␕' },  //     ^U              Negative Acknowledgement
        { 22,   L'␖' },  //     ^V              Synchronous Idle
        { 23,   L'␗' },  //     ^W              End of Transmission Block
        { 24,   L'␘' },  //     ^X              Cancel
        { 25,   L'␙' },  //     ^Y              End of Medium
        { 26,   L'␚' },  //     ^Z              Substitute
        { 27,   L'␛' },  //     ^[      \e      Escape
        { 28,   L'␜' },  //     ^\              File Separator
        { 29,   L'␝' },  //     ^]              Group Separator
        { 30,   L'␞' },  //     ^^              Record Separator
        { 31,   L'␟' },  //     ^_              Unit Separator
        { 32,   L'␠' },  //     Space (invisible but not unprintable !)
        {127,   L'␡' },  //     ^?              Delete
      };


    auto it = k_escapes.find(ch);
    if (it != k_escapes.end()) {
      // found
      return it->second;
    } else {
      // not found..is it printable?
      if (! isprint(ch)) {
        return L'�';
      } else {
        return ch;
      }
    }
  }
  // NOLINTEND
#pragma GCC diagnostic pop

}; // end class fixed_size_string_buffer_utils

///
/// @brief for debugging purposes, dump internal state of queue
///
///
/// @code
///   std::cout << rb;
///   rb.dump(std::cout);
/// @endcode
/// 
template <size_t SPACE>
std::ostream &operator<<(std::ostream &os, FixedSizeStringBuffer<SPACE> &rb)
{
  rb.dump(os);
  return os;
}

template <size_t SPACE>
void FixedSizeStringBuffer<SPACE>::dump_long_str(std::ostream &os) const
{
  constexpr auto ws2s = fixed_size_string_buffer_utils::ws2s;
  constexpr auto escaped = fixed_size_string_buffer_utils::escaped;

  for (size_t i = 0; i < max_chars_; i++) {
    std::string cstr = ws2s(std::wstring{escaped(chars_[i])});
    os << "  c[" << std::setw(2) << i << "] = " << cstr;
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
void FixedSizeStringBuffer<SPACE>::dump_short_str(std::ostream &os) 
{
    
  SlotState slot = mark_open_close_slots();

  print_box_line(os, slot, true);
  print_char_line(os, slot);
  print_box_line(os, slot, false);

}

//
// each char has 3 slots:
//  [open][word][close]
//
// open slot: marker for start of string
// word slot: characters of string
// clos slot: marker for end of sting
//
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
    size_t last = (end == 0) ? max_chars_ - 1 : end - 1;

    slot.bopen[start] = true;
    slot.bclos[last] = true;

    if (end > start) {
      for (size_t i = start; i < end; i++) {
        slot.bword[i] = true;
      }
    } else {
      for (size_t i = start; i < max_chars_; i++) {
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
  constexpr auto ws2s = fixed_size_string_buffer_utils::ws2s;
  enum class CT {left, open, close, dash, space, right};
  typedef std::unordered_map<CT,wchar_t> box_t;

  static const box_t box_top = {
      {CT::left,  L'⎧'},
      {CT::open,  L'╭'},
      {CT::close, L'╮'},
      {CT::dash,  L'─'},
      {CT::space, L' '},
      {CT::right, L'⎫'},
   };

  static const box_t box_bot = {
      {CT::left,  L'⎩'},
      {CT::open,  L'╰'},
      {CT::close, L'╯'},
      {CT::dash,  L'─'},
      {CT::space, L' '},
      {CT::right, L'⎭'},
   };


  box_t box = is_top ? box_top : box_bot;

  // insert space on left
  wchar_t cleft = box[CT::left];
  std::wstring wleft{cleft};
  os << "           " << ws2s(wleft) << ' ';
  // characters
  for (size_t i = 0; i < max_chars_; i++) {
    wchar_t copen = box[CT::space];
    wchar_t cchar = box[CT::space];
    wchar_t cclos = box[CT::space];

    if (slot.bword[i]) {
      copen = box[CT::dash];
      cchar = box[CT::dash];
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
  constexpr auto ws2s = fixed_size_string_buffer_utils::ws2s;
  constexpr auto escaped = fixed_size_string_buffer_utils::escaped;

  os << " buf[" << std::setw(2) << max_chars_ << "] = ⎨ ";
  for (size_t i = 0; i < max_chars_; i++) {
    // 3 slots for each char: [close][open][letter]
    wchar_t copen = slot.bopen[i] ? L'│' : L' ';
    wchar_t cchar = escaped(chars_[i]);
    wchar_t cclos = slot.bclos[i] ? L'│' : L' ';
    // mark first element and end of last element in queue
    if (! empty()) {
      if (i == ptr_[0].front) {
        copen = L'┤';
      }
      size_t back_left = (back_ == 0) ? max_chars_ - 1 : back_ - 1;
      if (i == back_left) {
        cclos = L'├';
      }
    }

    auto wline = std::wstring{copen, cchar, cclos};
    os << ws2s(wline);
  }
  os << "⎬ \n";

}


} // namespace fssb
