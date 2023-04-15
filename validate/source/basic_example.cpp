#include <iostream>
#include "fssb/fixed_size_string_buffer.h"

int main() {
  auto foo = fssb::FixedSizeStringBuffer<8>();

  // push strings into buffer
  std::cout << foo;
  return 0;
}
