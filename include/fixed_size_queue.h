
/*
 from https://stackoverflow.com/questions/56334492/c-create-fixed-size-queue
 Usage:
    FixedQueue<string, 100> cmdHistory;
*/
#pragma once

#include <queue>
#include <utility>
#include <iostream>

namespace {
struct Element {
  std::string str;
  size_t len;
  Element(std::string_view e_str, size_t e_len)
      : str(e_str), len(e_len)
  { }
};
}

class FixedQueue : public std::queue<Element> {

private:
  size_t free_space_ = 0;
  //using std::queue<Element>::push_range; // disallow push_range

public:
  explicit FixedQueue(size_t SIZE)
      : free_space_(SIZE)
  {
  }

  void push(const std::string& str);  
  std::string pop();

  template <class T>
  void emplace (T str) {
    push(std::string(str));
  }

  std::string front() {
    return std::queue<Element>::front().str;
  }
  std::string back() {
    return std::queue<Element>::back().str;
  }
};


inline void FixedQueue::push(const std::string& str) 
{
  // clear space for str
  size_t strlen = str.length();
  while (free_space_ < strlen) {
    free_space_ += std::queue<Element>::front().len;
    std::queue<Element>::pop();
  }

  std::queue<Element>::emplace(str, strlen);
  free_space_ -= strlen;
}

inline std::string FixedQueue::pop() 
{
  auto elem = std::queue<Element>::front();
  std::queue<Element>::pop(); 
  free_space_ += elem.len;
  return elem.str;
}

