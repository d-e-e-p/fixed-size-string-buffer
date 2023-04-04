//#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
//#pragma clang diagnostic ignored "-Wold-style-cast"
//#pragma clang diagnostic ignored "-Wnarrowing"
//#pragma execution_character_set("utf-8")

#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "fixed_size_string_buffer.h"
#include "fixed_elem_size_queue.h"
#include "fixed_char_size_queue.h"

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
auto time_queue(T& q, std::string& str_test) 
{

  using std::chrono::duration_cast;
  using std::chrono::nanoseconds;
  using std::chrono::steady_clock;

  constexpr int num_iter = 1000 * 1000;

  auto t1 = steady_clock::now();
  for (auto i = 0; i < num_iter; ++i) { 
    q.push(str_test); 
  }
  auto t2 = steady_clock::now();
  auto delta = duration_cast<nanoseconds>(t2 - t1).count() / num_iter;

  return delta;
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

  auto buf1 = FixedSizeStringBuffer<max_size>();        // opt1: ring buffer
  auto buf2 = FixedCharSizeQueue(max_size);             // opt2: char limit
  auto buf3 = FixedElemSizeQueue<std::string>(CAPACITY);// opt3: string limit

  // time options
  auto delta1 = time_queue(buf1, str_test);
  auto delta2 = time_queue(buf2, str_test);
  auto delta3 = time_queue(buf3, str_test);


  auto baseline = static_cast<long double>(delta1);

  auto ratio1 = static_cast<long double>(delta1) / baseline;
  auto ratio2 = static_cast<long double>(delta2) / baseline;
  auto ratio3 = static_cast<long double>(delta3) / baseline;

  using std::setw;

  std::cout.precision(1);
  std::cout << " │ " << setw(6) << LEN << " │ " << setw(8) <<  max_size << " │ "
    << setw(6) << delta1 << "ns │" << setw(6) << delta2 << "ns │" << setw(6) << delta3 << "ns │" 
    << std::fixed 
    << setw(5) << ratio1 << "X │" << setw(5) << ratio2 << "X │" << setw(5) << ratio3 << "X │" 
    << "\n";

}

void example2()
{
  std::cout << R"(
   fixed_size_string_buffer : wallclock time comparison for push operation
 ╭────────┬──────────┬──────────┬─────────┬─────────┬───────────────────────╮
 │ strlen │ max_size │ FixedSize│FixedChar│FixedStr │      R A T I O S      │
 │ (chars)│  (chars) │ stringBuf│std:queue│std:queue┼───────┬───────┬───────┤
 │        │          │    (1)   │   (2)   │   (3)   │(1)/(1)│(2)/(1)│(3)/(1)│
 ├────────┼──────────┼──────────┼─────────┼─────────┼───────┼───────┼───────┤
)";
  // NOLINTBEGIN
  compare<10,10,3>();
  compare<100,10,3>();
  compare<1000,10,3>();
  // NOLINTEND
  std::cout << 
R"( ╰────────┴──────────┴──────────┴─────────┴─────────┴───────┴───────┴───────╯
     (1)  FixedSizeStringBuffer<max_size>()
     (2)  FixedCharSizeQueue(max_size)
     (3)  FixedElemSizeQueue<std::string>(10)

          max_size = strlen * 10.3 chars

)";
}

