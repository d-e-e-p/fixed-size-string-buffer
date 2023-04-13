/**
 * @file  fixed_elem_size_queue.h
 * @author  Sandeep <deep@tensorfield.ag>
 * @version 1.0
 *
 * @section LICENSE
 *
 * MIT License <http://opensource.org/licenses/MIT>
 *
 * @section DESCRIPTION
 *
 * A std:queue with fixed size that you insiantiate like:
 *    auto q = FixedElemSizeQueue<std::string>(size);
 *
 * See discussion at:
 *
 *    from https://stackoverflow.com/questions/56334492/c-create-fixed-size-queue
 *
 * https://github.com/d-e-e-p/fixed-size-string-buffer
 * Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
 * 
 */

/*
 Usage:
    auto buf3 = FixedElemSizeQueue<std::string>(CAPACITY);
*/


#pragma once

#include <queue>

namespace fssb {

template <typename T>
class FixedElemSizeQueue : public std::queue<T> {

private:
  size_t free_space_ = 0;

public:
  explicit FixedElemSizeQueue(size_t max_elements)
      : free_space_(max_elements)
  {
  }

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
