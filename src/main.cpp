#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wnarrowing"

#include <array>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <iomanip>
#include <vector>

#include "fixed_size_string_buffer.h"
#include "fixed_size_queue.h"

void example1()
{
  std::cout << " fixed_size_string_buffer example1 \n";
  constexpr size_t max_size = 10;
  auto rb = FixedSizeStringBuffer<max_size>();
  rb.set_debug(true);
  assert(rb.free_space() == max_size);
  assert(rb.empty());
  const std::string_view msg = "0123456789";
  rb.push(msg);
  assert(rb.front() == msg);
  assert(rb.back() == msg);
  rb.push("0aa");
  assert(rb.free_space() == 7);
  rb.pop();
  assert(rb.size() == 0);
  rb.clear();
  rb.push("0aa");
  rb.push("1bb");
  rb.push("2cc");
  rb.push("3dd");
  rb.push("4ee");
  rb.push("5ff");
  std::cout << rb << "\n";
  assert(rb[0] == "3dd");
  assert(rb[1] == "4ee");
  assert(rb[2] == "5ff");
}

template <size_t LEN, size_t CAPACITY, size_t EXCESS>
void compare()
{
  using std::chrono::duration_cast;
  using std::chrono::nanoseconds;
  using std::chrono::steady_clock;

  constexpr std::string_view long_str = R"(
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
commodo consequat. Duis aute irure dolor in reprehenderit in voluptate
velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint
occaecat cupidatat non proident, sunt in culpa qui officia deserunt
mollit anim id est laborum.
)";
  auto str_test = std::string(LEN, 'x');

  // create buffer just enough size to hold str_capacity_in_buffer values
  constexpr double str_capacity_in_buffer = CAPACITY + 0.1 * EXCESS;
  constexpr auto max_size = static_cast<size_t>(LEN * str_capacity_in_buffer);
  auto rfb = FixedSizeStringBuffer<max_size>();

  constexpr int trials = 1000 * 1000;
  auto t1 = steady_clock::now();

  for (auto i = 0; i < trials; ++i) { 
    rfb.push(str_test); 
  }
  auto t2 = steady_clock::now();

  auto delta1 = duration_cast<nanoseconds>(t2 - t1).count() / trials;

  auto rfq = FixedQueue(max_size);
  const auto str_test2 = std::string(str_test);

  t1 = steady_clock::now();
  for (auto i = 0; i < trials; ++i) { 
    rfq.push(str_test2); 
  }
  t2 = steady_clock::now();

  auto delta2 = duration_cast<nanoseconds>(t2 - t1).count() / trials;

  std::queue<std::string> rq = {};

  t1 = steady_clock::now();
  for (auto i = 0; i < trials; ++i) { 
    rq.push(str_test2); 
  }
  t2 = steady_clock::now();

  auto delta3 = duration_cast<nanoseconds>(t2 - t1).count() / trials;

  std::cout.precision(1);
  std::cout << " | " << std::setw(6) << LEN << " | " << std::setw(6) <<  CAPACITY << "." << EXCESS << " | " 
    << std::setw(6) << delta1 << "ns |" << std::setw(6) << delta2 << "ns |" << std::setw(6) << delta3 << "ns |" 
   << std::fixed << std::setw(7) << 1.0 << "X |" << std::setw(7) << (long double) delta2 / delta1 << "X |" << std::setw(7) << (long double) delta3 / delta1 << "X |" << "\n";

  auto num = static_cast<size_t>(str_capacity_in_buffer);
  assert(rfb.size() == num);
  for (size_t i = 0; i < num; ++i) { 
    assert(rfb[i] == str_test); 
  }
}

void example2()
{
  std::cout << R"(
fixed_size_string_buffer eg2: push time comparison
 +--------------------------------------------------------------------------------+
 | strlen | capacity | FixedSize|FixedSize|std:queue|         |         |         |
 : (chars)| (strings)| stringBuf|stdqueue | no limit|         |         |         |
 +--------------------------------------------------------------------------------+
)";
  compare<1,10,3>();
  compare<10,10,3>();
  compare<100,10,3>();
  compare<1000,10,3>();
  std::cout << 
    " +--------------------------------------------------------------------------------+\n";
}


int main()
{
  example1();
  example2();
 
  return 0;
}
