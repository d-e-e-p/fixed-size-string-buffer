//#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
//#pragma clang diagnostic ignored "-Wold-style-cast"
//#pragma clang diagnostic ignored "-Wnarrowing"

#define UNICODE 
#define _UNICODE 
#pragma execution_character_set("utf-8")

#undef NDEBUG // allow assert
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <array>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

#include "fixed_size_string_buffer.h"
#include "fixed_size_queue.h"

void example1()
{
  std::cout << "fixed_size_string_buffer example1 \n\n";
  constexpr size_t max_size = 10;
  auto rb = FixedSizeStringBuffer<max_size>();
  rb.set_debug(true);
  std::cout << " created ring buffer of " << max_size << " characters\n";

  // add strings
  const std::string str = "The Quick Brown Fox Jumped Over The Lazy Dog";
  std::cout << " adding words to buffer from: '" << str << "\n";
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

  auto ratio2 = static_cast<long double>(delta2) / static_cast<long double>(delta1);
  auto ratio3 = static_cast<long double>(delta3) / static_cast<long double>(delta1);

  std::cout.precision(1);
  std::cout << " │ " << std::setw(6) << LEN << " │ " << std::setw(6) <<  CAPACITY << "." << EXCESS << " │ " 
    << std::setw(6) << delta1 << "ns │" << std::setw(6) << delta2 << "ns │" << std::setw(6) << delta3 << "ns │" 
   << std::fixed << std::setw(7) << 1.0 << "X │" << std::setw(7) << ratio2 << "X │" << std::setw(7) << ratio3 << "X │" << "\n";

  auto num = static_cast<size_t>(str_capacity_in_buffer);
  assert(rfb.size() == num);
  for (size_t i = 0; i < num; ++i) { 
    assert(rfb[i] == str_test); 
  }
}

void example2()
{
  std::cout << R"(
  fixed_size_string_buffer example2: wallclock time comparison for push operation
 ╭────────┬──────────┬──────────┬─────────┬─────────┬─────────┬─────────┬─────────╮
 │ strlen │ capacity │ FixedSize│FixedSize│no limit │         │         │         │
 │ (chars)│ (strings)│ stringBuf│std:queue│std:queue│ (1)/(1) │ (2)/(1) │ (3)/(1) │
 │        │          │    (1)   │   (2)   │  (3)    │         │         │         │
 ├────────┼──────────┼──────────┼─────────┼─────────┼─────────┼─────────┼─────────┤
)";
  // NOLINTBEGIN
  compare<1,10,3>();
  compare<10,10,3>();
  compare<100,10,3>();
  compare<1000,10,3>();
  // NOLINTEND
  std::cout << 
    " ╰────────┴──────────┴──────────┴─────────┴─────────┴─────────┴─────────┴─────────╯\n";
}

/*
#include <string>
#include <iostream>
#include <Windows.h>
#include <cstdio>

#include <io.h>    // for _setmode()
#include <fcntl.h> // for _O_U16TEXT


void example3()
{

    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 1000);
	  _setmode(_fileno(stdout), _O_U16TEXT);

    std::string test = u8"Greek: αβγδ; German: Übergrößenträger";
    std::cout << test << std::endl;
}
*/

#include <clocale>
#include <iostream>
#include <cstdio>
#include <locale>
#include <windows.h>


void example4() {
  std::setlocale(LC_ALL, ".UTF8");
  SetConsoleOutputCP(CP_UTF8);
  system("chcp.com 65001");
  system("echo Ιλιάδα");
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
  printf( "Testing unicode -- English -- Ελληνικά -- Español -- Русский. aäbcdefghijklmnoöpqrsßtuüvwxyz\n" );


  std::cout << " example 4 ...........................\n";
  std::wcout << L'\u2780' << std::endl;
  std::string test = "Greek: αβγδ; German: Übergrößenträger";
  std::cout << test << std::endl;
}


int main()
{
  //example4();
  //example3();
  example1();
  example2();
 
  return 0;
}
