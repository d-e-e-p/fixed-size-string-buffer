/**
 * @file fixed_char_size_queue.h
 * A string queue with upper limit on number of characters:
 * @code
 *    auto buf = fssb::FixedCharSizeQueue(max_size);
 * @endcode
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
 * Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
 *
 */

#pragma once

#include <queue>
#include <utility>
#include <iostream>

/// @namespace fssb
/// @brief fssb
namespace fssb {

// the string and a cache of it's length
/// @struct Element
/// @brief string and cache strlen
struct Element {
  std::string str;  
  size_t len;       
  Element(std::string_view e_str, size_t e_len)
      : str(e_str), len(e_len)
  { }
};


/// @class FixedCharSizeQueue
/// @brief A string queue with upper limit on number of characters:
///
/// Example usage:
/// @code{.cpp}
/// #include "fssb/fixed_char_size_queue.h"
/// int main() {
///   auto rb = FixedCharSizeQueue(10);
///   rb.push("123");
///   rb.push("456");
///   rb.pop();
///   return 0;
/// }
/// @endcode
///
///
class FixedCharSizeQueue : public std::queue<Element> {

private:
  size_t free_space_ = 0;
  size_t max_chars_ = 0;
  //using std::queue<Element>::push_range; // disallow push_range
public:
  
  ///  @brief Constructor that creates a string buffer of fixed character size
  ///  @param[in] SIZE max char size of queue
  explicit FixedCharSizeQueue(size_t SIZE)
      : free_space_(SIZE), max_chars_(SIZE)
  {
  }

  void push(const std::string& str);  ///< @brief add string to back of queue
  std::string pop();                  ///< @brief remove string from front of queue

  /// @brief push new entry into queue using any std::string constructor arguments
  template <typename... Args>
  void emplace(Args&&... args) {
    push(std::string(std::forward<Args>(args)...));
  }

  /// @brief oldest element still in buffer
  std::string front() {
    return std::queue<Element>::front().str;
  }
  /// @brief newest element pushed into buffer
  std::string back() {
    return std::queue<Element>::back().str;
  }
};

///
/// push first creates space in buffer by silently removing oldest
/// string in queue until there is enough space.
///
/// @param[in] str string to push into queue
/// @warning attemping to push a string larger than capacity fails
/// just a with warning message, and queue is unchanged
///
inline void FixedCharSizeQueue::push(const std::string& str) 
{
  // clear space for str
  size_t strlen = str.length();
  if (strlen > max_chars_) {
    std::string msg = "string length : " + std::to_string(strlen) +
                      " > max size " + std::to_string(max_chars_);
    std::cerr << msg << "\n";
    return;
  } 
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

} // namespace fssb
