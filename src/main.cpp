//#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
//#pragma clang diagnostic ignored "-Wold-style-cast"
//#pragma clang diagnostic ignored "-Wnarrowing"
//#pragma execution_character_set("utf-8")

#undef NDEBUG // allow assert

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "fixed_size_string_buffer.h"
#include "fixed_size_queue.h"

void example1();
void example2();

int main()
{
  example1();
  example2();
 
  return 0;
}

void example1()
{
  std::cout << "fixed_size_string_buffer example1 \n\n";
  constexpr size_t max_size = 10;
  auto rb = FixedSizeStringBuffer<max_size>();
  rb.set_debug(true);
  std::cout << " created ring buffer of " << max_size << " characters\n";

  // add strings
  const std::string str = "The Quick Brown Fox Jumped Over The Lazy Dog";
  std::cout << " adding words to buffer from: '" << str << "'\n";
  std::istringstream ss(str);
  std::string word;
  std::cout << rb;
  while (ss >> word) {
    rb.push(word);
    std::cout << rb;
  }
                        
  std::cout << " buffer free space is " << rb.free_space() << " characters\n";
  std::cout << " pop() removing oldest surviving string: '" << rb.pop() << "'\n";
  std::cout << " so now buffer looks like:\n\n";
  std::cout << rb << "\n";

  std::cout << " and buffer free space is " << rb.free_space() << " characters\n";

  // iterate over all enteries in buffer
  while (! rb.empty()) {
    rb.pop();
  }
  std::cout << " result of pop() on all entries: \n\n";
  std::cout << rb << "\n";
  std::cout << " result of clear(): \n\n";
  rb.clear();
  std::cout << rb << "\n";

}

/*
 * compare template has 3 params:
 *  LEN: length of test string used for benchmark
 *  CAPACITY: how many strings can the buffer hold
 *  EXCESS: 3 means additional buffer size if 0.3 * stringsize
 */


template <class T> 
auto time_queue(T& q_empty, std::string& str_test) 
{

  using std::chrono::duration_cast;
  using std::chrono::nanoseconds;
  using std::chrono::steady_clock;

  constexpr int n_outer = 1000;
  constexpr int n_inner = 1000;

  long long total_delta = 0;

  for (auto i = 0; i < n_outer; ++i) { 
    auto q_test = q_empty;

    auto t1 = steady_clock::now();
    for (auto j = 0; j < n_inner; ++j) { 
      q_test.push(str_test); 
    }
    auto t2 = steady_clock::now();
    auto delta = duration_cast<nanoseconds>(t2 - t1).count() / n_inner;
    total_delta += delta;
  }

  return total_delta / n_outer;
}


template <size_t LEN, size_t CAPACITY, size_t EXCESS>
void compare()
{
  using std::chrono::duration_cast;
  using std::chrono::nanoseconds;
  using std::chrono::steady_clock;

  auto str_test = std::string(LEN, 'x');

  // create buffer just enough size to hold str_capacity_in_buffer values
  constexpr double str_capacity_in_buffer = CAPACITY + 0.1 * EXCESS;
  constexpr auto max_size = static_cast<size_t>(LEN * str_capacity_in_buffer);

  auto buf1 = FixedSizeStringBuffer<max_size>(); // opt1: FixedString
  auto buf2 = FixedQueue(max_size);              // opt2: FixedQueue
  auto buf3 = std::queue<std::string>();         // opt3: std::queue

  // time options
  auto delta1 = time_queue(buf1, str_test);
  auto delta2 = time_queue(buf2, str_test);
  auto delta3 = time_queue(buf3, str_test);


  auto ratio1 = static_cast<long double>(delta1) / static_cast<long double>(delta1);
  auto ratio2 = static_cast<long double>(delta2) / static_cast<long double>(delta1);
  auto ratio3 = static_cast<long double>(delta3) / static_cast<long double>(delta1);

  std::cout.precision(1);
  std::cout << " │ " << std::setw(6) << LEN << " │ " << std::setw(8) <<  max_size << " │ " 
    << std::setw(6) << delta1 << "ns │" << std::setw(6) << delta2 << "ns │" << std::setw(6) << delta3 << "ns │" 
   << std::fixed << std::setw(7) << ratio1 << "X │" << std::setw(7) << ratio2 << "X │" << std::setw(7) << ratio3 << "X │" << "\n";

}

void example2()
{
  std::cout << R"(
  fixed_size_string_buffer example2: wallclock time comparison for push operation
 ╭────────┬──────────┬──────────┬─────────┬─────────┬─────────────────────────────╮
 │ strlen │ max_size │ FixedSize│FixedSize│no limit │         R A T I O S         │
 │ (chars)│  (chars) │ stringBuf│std:queue│std:queue┼─────────┬─────────┬─────────┤
 │        │          │    (1)   │   (2)   │  (3)    │ (1)/(1) │ (2)/(1) │ (3)/(1) │
 ├────────┼──────────┼──────────┼─────────┼─────────┼─────────┼─────────┼─────────┤
)";
  // NOLINTBEGIN
  compare<10,10,3>();
  compare<100,10,3>();
  compare<1000,10,3>();
  // NOLINTEND
  std::cout << 
R"( ╰────────┴──────────┴──────────┴─────────┴─────────┴─────────┴─────────┴─────────╯
     (1)  FixedSizeStringBuffer<max_size>()
     (2)  FixedQueue(max_size)
     (3)  std::queue<std::string>

          max_size = strlen * 10.3

)";
}

