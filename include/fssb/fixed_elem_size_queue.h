/**
 * @file  fixed_elem_size_queue.h
 * A std:queue with fixed size that you insiantiate like:
 * @code
 *    auto q = fssb::FixedElemSizeQueue<std::string>(size);
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
 * See discussion at:
 *
 *    from https://stackoverflow.com/questions/56334492/c-create-fixed-size-queue
 *
 * https://github.com/d-e-e-p/fixed-size-string-buffer
 * Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
 * 
 */


#pragma once

#include <queue>

/// @namespace fssb
/// @brief fssb
namespace fssb {

/// @class FixedElemSizeQueue
/// @brief A templated queue with upper limit on number of elements
/// @tparam T type of element
///
/// Example usage:
/// @code{.cpp}
/// #include "fssb/fixed_elem_size_queue.h"
/// int main() {
///   auto rb = FixedElemSizeQueue<std::string>(10);
///   rb.push("123");
///   rb.push("456");
///   rb.pop();
///   return 0;
/// }
/// @endcode
///
///
template <typename T>
class FixedElemSizeQueue : public std::queue<T> {

private:
  size_t free_space_ = 0;

public:
  ///  @brief Constructor that creates a buffer of fixed number of elements 
  ///  @param[in] max_elements max num of elements in queue
  explicit FixedElemSizeQueue(size_t max_elements)
      : free_space_(max_elements)
  {
  }

  ///  @brief add elements to back of queue, 
  ///  silently pop() if queue has reached max_elements capacity
  ///  @param[in] value element to add to queue
  void push(const T& value) {
    if (free_space_ == 0) {
       pop();
    }
    std::queue<T>::push(value);
    free_space_ --;
  }

  T pop() {
    auto elem = this->front();
    std::queue<T>::pop();
    free_space_ ++;
    return elem;
  }

};

} // namespace fssb
