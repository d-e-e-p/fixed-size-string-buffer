
/*
 from https://stackoverflow.com/questions/56334492/c-create-fixed-size-queue
 Usage:
    FixedElemSizeQueue<string> cmdHistory(20);
*/


#pragma once

#include <deque>
#include <iostream>

template <typename T>
class FixedElemSizeQueue : public std::deque<T> {

private:
  size_t free_space_ = 0;

public:
  explicit FixedElemSizeQueue(size_t SIZE)
      : free_space_(SIZE)
  {
    std::deque<T>::resize(SIZE);
    std::deque<T>::clear();
  }

  void push(const T& value) {
    if (free_space_ == 0) {
       std::deque<T>::pop_front();
    }
    std::deque<T>::push_back(value);
  }

};


