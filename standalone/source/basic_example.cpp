#include <iostream>
#include "fssb/fixed_size_string_buffer.h"

int main() {
  auto foo = FixedSizeStringBuffer<8>();

  // push strings into buffer
  std::cout << foo;
  foo.push("0aa"); std::cout << foo;
  foo.push("1bb"); std::cout << foo;
  foo.push("2cc"); std::cout << foo;

  // inspect  buffer
  std::cout << "foo.front(): " << foo.front()  << "\n";
  std::cout << "foo.back() : " << foo.back()   << "\n";
  std::cout << "foo[0]     : " << foo[0]       << "\n";
  std::cout << "foo.pop()  : " << foo.pop()    << "\n";
  std::cout << foo << "\n";
  std::cout << "foo[0]     : " << foo[0]       << "\n";
  return 0;
}
