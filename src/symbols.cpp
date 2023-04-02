#include <iostream>
#include <array>
#include "fixed_size_string_buffer.h"

/*
 * test out printing of queues with special characters
 */

const size_t num = 25;
auto foo = FixedSizeStringBuffer<num>();

template <class T>
void print(T buf) {
  std::cout << buf  << "\n";
}

int main() {
  std::cout << foo << "\n";

  // push strings into buffer
  print(foo);
  foo.push("1\t✊\n"); print(foo);
  foo.push("2✋\t"); print(foo);
  foo.push("3✌️\r\n"); 
  // inspect them
  foo.dump_long_str(std::cout);
  std::cout << foo << "\n";
  std::cout << "foo.front(): " << foo.front()  << "\n";
  std::cout << "foo.back() : " << foo.back()   << "\n"; 
  std::cout << "foo[0]     : " << foo[0]       << "\n";     
  std::cout << "foo.pop()  : " << foo.pop()    << "\n";  
  std::cout << "foo[0]     : " << foo[0]       << "\n";     
  return 0;
}
