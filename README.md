
# A pre-allocated ring buffer for std::string messages

C++ Header-only library 
[fixed_size_string_buffer.h](include/fixed_size_string_buffer.h) that instantiates
a char array sized at compile-time. The queue stores string messages in a char ring buffer.
When this buffer is full, oldest strings are deleted to make way for the new entries.
Requires at least -std=c++17 . 

This is a small enough example to clone and use as a template to setup a new C++ project with:
[cmake](https://cmake.org/cmake/help/latest/), 
[GoogleTest](https://google.github.io/googletest/), 
[GoogleBenchmark](https://github.com/google/benchmark), 
[coverage](https://github.com/linux-test-project/lcov), 
[doxygen](https://www.doxygen.nl/), 
[github action](https://github.com/d-e-e-p/fixed-size-string-buffer/actions), 
[unicode on windows](#Porting),
[clang-tidy](https://clang.llvm.org/extra/clang-tidy/), 
[cppcheck](https://cppcheck.sourceforge.io/) etc.

## Credits

- Cmake and other setup from [ModernCppStarter](https://github.com/TheLartians/ModernCppStarter)
- Compiler/Analyzer template from [modern-cpp-template](https://github.com/filipdutescu/modern-cpp-template)
- Setting and conventions from [p-ranav](https://github.com/p-ranav)
- Google test and benchmark example from [starter project](https://github.com/PhDP/cmake-gtest-gbench-starter)
- Discussion at stackexchange [Elegant Circular Buffer](https://codereview.stackexchange.com/questions/164130/elegant-circular-buffer)

## Motivation

Some applications need a fixed-size [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer)
with automatic overwrite of tail messages by the head.  
If the sizes of strings were known, then one way to achive this would be to 
budget for a target capacity and then pop-on-push:
```cpp
std::deque<std::string> q;
...
if (q.size() > target_capacity) {
   q.pop_front();
 }
 q.push_back(new_string);
```

See [fixed_elem_size_queue.h](include/fssb/fixed_elem_size_queue.h)
You could extend this to take into account the sizes of strings added, eg
[fixed_char_size_queue.h](include/fssb/fixed_char_size_queue.h)
This achieves pretty much the same functionality of this library except the requirement to 
pre-allocate buffer space at compile time.

Turns out that feature can be pretty useful.  For example, in embeded devices we need very predictable
dynamic allocation limits to maintain sufficient memory headroom.  This ring buffer array can be 
allocated statically so it ends up in the [.bss section](https://en.wikipedia.org/wiki/.bss), 
when can be alloted to a dedicated bank (eg CCM Memory on a STM32). This eliminates the possibility
of conflict between message buffer and operating heap/stack memory.  See writeup on [Using CCM
Memory](https://www.openstm32.org/Using%2BCCM%2BMemory). 

There is also a significant speed advantage of using this approach for long strings, eg on macos:

```bash
   fixed_size_string_buffer : wallclock time comparison for push operation
 ╭────────┬──────────┬──────────┬─────────┬─────────┬───────────────────────╮
 │ strlen │ max_size │ FixedSize│FixedChar│FixedStr │      R A T I O S      │
 │ (chars)│  (chars) │ stringBuf│std:queue│std:queue┼───────┬───────┬───────┤
 │        │          │    (1)   │   (2)   │   (3)   │(1)/(1)│(2)/(1)│(3)/(1)│
 ├────────┼──────────┼──────────┼─────────┼─────────┼───────┼───────┼───────┤
 │     10 │      103 │      7ns │     6ns │     7ns │  1.0X │  0.9X │  1.0X │
 │    100 │     1030 │      7ns │    24ns │    48ns │  1.0X │  3.4X │  6.9X │
 │   1000 │    10300 │     18ns │    67ns │   141ns │  1.0X │  3.7X │  7.8X │
 ╰────────┴──────────┴──────────┴─────────┴─────────┴───────┴───────┴───────╯
     (1)  FixedSizeStringBuffer<max_size>()
     (2)  FixedCharSizeQueue(max_size)
     (3)  FixedElemSizeQueue<std::string>(10)

          max_size = strlen * 10.3 chars
```

This run shows a 3.4X speedup for strings longer than 100 characters, compared with a std::queue<string> based approach.
As strings become longer, the overhead of using bounded or unbounded string std::queue becomes even more pronounced.
Ring buffer wins over unbounded queues because it avoids the extra memory allocation time..

## Getting Started

`make help` gives the following options:

```bash
help                 this message
clean                remove build dir
debug                create slow debug version of standalone example
release              create optimized release version of example
bench                run benchmark on push operation under bench/sources
test                 exercise all queue operations under test/sources
coverage             check code coverage
docs                 generate Doxygen HTML documentation, including API docs
install              install the package to the INSTALL_LOCATION=~/.local
format               format the project sources
```

`make release` builds the example [main.cpp](standalone/source/main.cpp)

A trivial example looks like:

```cpp
#include "fssb/fixed_size_string_buffer.h"
int main() {
  auto rb = FixedSizeStringBuffer<10>();
  rb.push("123");
  rb.push("456");
  rb.pop();
  return 0;
}
```

then compiled with:

```bash
g++ -std=c++17 -I include test.cpp
./a.out
```

## API

Externally this class looks like a simple queue with infinite space.
In this example, three sets of 3-char messages are stuffed into a buffer 
that can only hold 8 chars. `0aa` gets pushed out to make room for `2cc` 
because buffer doesn't have enough space for 9 chars.

```cpp
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
```

the result of running this should look like:

```bash
           ⎧                         ⎫
 buf[ 8] = ⎨  •  •  •  •  •  •  •  • ⎬
           ⎩                         ⎭
           ⎧ ╭───────╮               ⎫
 buf[ 8] = ⎨ ┤0  a  a├ •  •  •  •  • ⎬
           ⎩ ╰───────╯               ⎭
           ⎧ ╭───────╮╭───────╮      ⎫
 buf[ 8] = ⎨ ┤0  a  a││1  b  b├ •  • ⎬
           ⎩ ╰───────╯╰───────╯      ⎭
           ⎧ ──╮      ╭───────╮╭─────⎫
 buf[ 8] = ⎨  c├ a  a ┤1  b  b││2  c ⎬
           ⎩ ──╯      ╰───────╯╰─────⎭
foo.front(): 1bb
foo.back() : 2cc
foo[0]     : 1bb
foo.pop()  : 1bb
           ⎧ ──╮               ╭─────⎫
 buf[ 8] = ⎨  c├ a  a  1  b  b ┤2  c ⎬
           ⎩ ──╯               ╰─────⎭

foo[0]     : 2cc
```

See example1 in [basic_example.cpp](standalone/source/basic_example.cpp) for a similar demo.
Run `make docs` to see doxygen version of class descriptions.

### Coverage

`make coverage` generates coverage information that should sometime like:

```bash
Filename Sort by name	Line Coverage Sort by line coverage	Functions Sort by function coverage
fixed_char_size_queue.h	    100.0% 100.0 %	  21 /  21	100.0 %	12 / 12
fixed_elem_size_queue.h	    100.0% 100.0 %	  16 /  16	100.0 %	 6 /  6
fixed_size_string_buffer.h	100.0% 100.0 %	 199 / 199	 99.0 %	95 / 96
Generated by: LCOV version 1.16
```

### Tests and Benchmark

`make test` invokes google test on [test/test_basic.cpp](test/test_basic.cpp) and [test/test_compare.cpp](test/test_compare.cpp)
Expected results look something like:
```bash
1: Test command: /Users/deep/build/tf/command_line/fixed-size-string-buffer/build/test/unit_tests
1: Working Directory: /Users/deep/build/tf/command_line/fixed-size-string-buffer/build/test
1: Test timeout computed to be: 10000000
1: [==========] Running 8 tests from 2 test suites.
1: [----------] Global test environment set-up.
1: [----------] 5 tests from QueueTest
1: [ RUN      ] QueueTest.Small
1: [       OK ] QueueTest.Small (0 ms)
1: [ RUN      ] QueueTest.Warnings
1: [       OK ] QueueTest.Warnings (0 ms)
1: [ RUN      ] QueueTest.Large
1: [       OK ] QueueTest.Large (0 ms)
1: [ RUN      ] QueueTest.Swap
1: [       OK ] QueueTest.Swap (0 ms)
1: [ RUN      ] QueueTest.Emplace
1: [       OK ] QueueTest.Emplace (0 ms)
1: [----------] 5 tests from QueueTest (0 ms total)
1:
1: [----------] 3 tests from Compare
1: [ RUN      ] Compare.WithFixedCharSizeQueue
1: [       OK ] Compare.WithFixedCharSizeQueue (0 ms)
1: [ RUN      ] Compare.WithFixedStrSizeQueue
1: [       OK ] Compare.WithFixedStrSizeQueue (0 ms)
1: [ RUN      ] Compare.WithUnlimitedQueue
1: [       OK ] Compare.WithUnlimitedQueue (0 ms)
1: [----------] 3 tests from Compare (0 ms total)
1:
1: [----------] Global test environment tear-down
1: [==========] 8 tests from 2 test suites ran. (0 ms total)
1: [  PASSED  ] 8 tests.
```

`make bench` runs trials on push operation under varying conditions of string length, eg:

```bash
./build/bench/unit_bench
Unable to determine clock rate from sysctl: hw.cpufrequency: No such file or directory
This does not affect benchmark measurements, only the metadata output.
2023-04-06T08:29:53-04:00
Running ./build/bench/unit_bench
Run on (10 X 24.0735 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB
  L1 Instruction 128 KiB
  L2 Unified 4096 KiB (x10)
Load Average: 3.86, 7.40, 7.12
-----------------------------------------------------------------------------------------------
Benchmark                                                     Time             CPU   Iterations
-----------------------------------------------------------------------------------------------
BM_queue<QType::FixedSizeStringBuffer , 10, 10, 3>         5.46 ns         5.45 ns    125279642
BM_queue<QType::FixedCharSizeQueue, 10, 10, 3>             6.76 ns         6.75 ns    104608763
BM_queue<QType::FixedElemSizeQueue, 10, 10, 3>             6.98 ns         6.97 ns    100048595
BM_queue<QType::StdQueue, 10, 10, 3>                       5.68 ns         5.01 ns    139472793
BM_queue<QType::FixedSizeStringBuffer , 100, 10, 3>        5.65 ns         5.64 ns    123504711
BM_queue<QType::FixedCharSizeQueue, 100, 10, 3>            26.1 ns         24.4 ns     28585196
BM_queue<QType::FixedElemSizeQueue, 100, 10, 3>            48.7 ns         48.6 ns     14253745
BM_queue<QType::StdQueue, 100, 10, 3>                      46.2 ns         38.2 ns     21016035
BM_queue<QType::FixedSizeStringBuffer , 1000, 10, 3>       16.4 ns         16.4 ns     42636131
BM_queue<QType::FixedCharSizeQueue, 1000, 10, 3>           68.2 ns         68.1 ns     10158472
BM_queue<QType::FixedElemSizeQueue, 1000, 10, 3>            139 ns          139 ns      4973039
BM_queue<QType::StdQueue, 1000, 10, 3>                      395 ns          160 ns      5182728
``` 

The last line for example is running a case where the string added to queue is 1000 chars, 
while the queue size is 10.3 * string length = 10300 chars.  
It takes 160ns per push using an unbounded `std::queue<std::string>`, which reduces to
68ns with the `std::queue` of fixed char length from [fixed_char_size_queue.h](include/fssb/fixed_char_size_queue.h).
Using ring buffer with this large string length drops the push time to 16ns.

### Porting

Linux and MacOS build fine, see [https://github.com/d-e-e-p/fixed-size-string-buffer/actions](https://github.com/d-e-e-p/fixed-size-string-buffer/actions)

On windows, compiling wth unicode in the source files is a bit fragile.
For MS Visual Code to handle unicode correctly on windows, we need 4 steps:

1. Update terminal codepage for output to support UTF8 (with `chcp.com 65001`)
2. Encode source files as UTF-8 *with* BOM
3. `#pragma execution_character_set("utf-8")` in include files
4. Compile with `/utf-8` option

See section on `if: runner.os == 'Windows'` in the action config 
[.github/workflows/ci.yml](.github/workflows/ci.yml). 


### Coding style

.clang-format for this code looks like
```bash
Language:        Cpp
BasedOnStyle:  Google
AllowShortBlocksOnASingleLine: true
AllowShortIfStatementsOnASingleLine: true
ColumnLimit:     80
BreakBeforeBraces: Stroustrup
```
Roughly following https://google.github.io/styleguide/cppguide.html except 
in this case all functions and variables are snake_case.


```cpp
# naming conventions
MyExcitingClass, MyExcitingEnum.
a_local_variable, a_struct_data_member, a_class_data_member_.

std::string table_name;  // OK - lowercase with underscore.

class TableInfo {
  ...
 private:
  std::string table_name_;  // OK - underscore at end.
};

// global constexpr or const,
const int kDaysInAWeek = 7;

// functions should generally with a capital letter and have a capital letter for each new word.
// in this case all functions are snake_case:
int count();
void set_count(int count);

// Namespace names are all lower-case, with words separated by underscores.


```


## Versioning

This project makes use of [SemVer](http://semver.org/) for versioning. A list of
existing versions can be found in the
[project's releases](https://github.com/d-e-e-p/fixed-size-string-buffer/releases).

## Authors

* **Sandeep** - [@d-e-e-p](https://github.com/d-e-e-p)

## License

The project is available under the [MIT](https://opensource.org/licenses/MIT) license.
See [LICENSE](LICENSE) file for details
