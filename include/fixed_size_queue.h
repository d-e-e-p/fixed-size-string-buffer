
/*
 from https://stackoverflow.com/questions/56334492/c-create-fixed-size-queue
 Usage:
    FixedQueue<string, 100> cmdHistory;
*/
#pragma once

#include <queue>
#include <utility>
#include <iostream>

class FixedQueue : public std::queue<std::string> {

private:
  size_t free_space_ = 0;
  std::queue<size_t> strsize_ = {}; // pointer to start/length of str in chars
  using std::queue<std::string>::swap; // disallow swap

public:
  explicit FixedQueue(size_t SIZE)
      : free_space_(SIZE)
  {
  }

  void push(const std::string& str);  
  void pop();
};


inline void FixedQueue::push(const std::string& str) 
{
  // clear space for str
  size_t strlen = str.length();
  while (free_space_ < strlen) {
    free_space_ += strsize_.front();
    strsize_.pop();
    std::queue<std::string>::pop();
  }

  strsize_.push(strlen);
  std::queue<std::string>::push(str);
  free_space_ -= strlen;
}

inline void FixedQueue::pop() 
{
  free_space_ += strsize_.front();
  strsize_.pop();
  std::queue<std::string>::pop(); 
}

