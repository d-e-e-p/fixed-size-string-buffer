#include <iostream>
#include "fixed_size_string_buffer.h"

/*
 * test out printing of queues with special characters
 */


int main() {

  constexpr size_t num = 25;
  auto foo = FixedSizeStringBuffer<num>();

  // push strings into buffer
  std::cout << foo;
  foo.push("1\t✊\n");  std::cout << foo;
  foo.push("2✋\t\v");  std::cout << foo;
  foo.push("3 ✌️\r\n"); std::cout << foo;
  foo.push("1\t✊\n");  std::cout << foo;

  return 0;
}
