#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wnarrowing"

#include <array>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <vector>

#include "fixed_size_string_buffer.hpp"

void example1()
{
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

void example2()
{
  using std::chrono::duration_cast;
  using std::chrono::nanoseconds;
  using std::chrono::steady_clock;

  constexpr std::string_view str_test = R"(
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim
veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea
commodo consequat. Duis aute irure dolor in reprehenderit in voluptate
velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint
occaecat cupidatat non proident, sunt in culpa qui officia deserunt
mollit anim id est laborum.
)";

  // create buffer just enough size to hold str_capacity_in_buffer values
  constexpr double str_capacity_in_buffer = 10.3;
  constexpr auto max_size =
      (size_t)(str_test.length() * str_capacity_in_buffer);
  auto rb = FixedSizeStringBuffer<max_size>();

  constexpr int trials = 100000;
  auto t1 = steady_clock::now();

  for (auto i = 0; i < trials; ++i) { rb.push(str_test); }
  auto t2 = steady_clock::now();

  auto delta1 = duration_cast<nanoseconds>(t2 - t1).count() / trials;
  std::cout << " average FixedSizeStringBuffer push time is " << delta1
            << "ns\n";

  std::deque<std::string> q;
  std::string str_test_for_q = std::string(str_test);
  for (auto i = 0; i < trials; ++i) {
    if (q.size() > str_capacity_in_buffer) { q.pop_front(); }
    q.push_back(str_test_for_q);
  }
  auto t3 = steady_clock::now();
  auto delta2 = duration_cast<nanoseconds>(t3 - t2).count() / trials;
  std::cout << " average std::deque push time is " << delta2 << "ns\n";
  std::cout << "  => speedup is " << (long double)delta2 / delta1 << " X\n";

  auto num = static_cast<size_t>(str_capacity_in_buffer);
  assert(rb.size() == num);
  for (size_t i = 0; i < num; ++i) { assert(rb[i] == str_test); }
}

int main()
{
  std::cout << " fixed_size_string_buffer example \n";
  example1();
  example2();
  return 0;
}
