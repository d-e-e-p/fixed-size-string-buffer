
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

- Original project template from [modern-cpp-template](https://github.com/filipdutescu/modern-cpp-template)
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

See this [gist](https://gist.github.com/d-e-e-p/fc2697bdef0faa11678fe034d44772d3) .
You could extend this to take into account the sizes of strings added, eg [fixed_size_queue.h](include/fixed_size_queue.h).
This achieves pretty much the same functionality of this library except the ability to 
pre-allocate buffer space on the stack at compile time.

Turns out that feature can be pretty useful.  For example, in embeded devices we need very predictable
dynamic allocation limits to maintain sufficient ram headroom.  This char buffer array can be 
allocated statically so it ends up in the [.bss section](https://en.wikipedia.org/wiki/.bss), 
when can be alloted to a dedicated bank (eg CCM Memory on a STM32). This eliminates the possibility
of conflict between message buffer and operating heap/stack memory.  See writeup on [Using CCM
Memory](https://www.openstm32.org/Using%2BCCM%2BMemory).

There is also a speed advantage of using this approach for long strings:

```bash
  fixed_size_string_buffer example2: wallclock time comparison for push operation
 ╭────────┬──────────┬──────────┬─────────┬─────────┬─────────┬─────────┬─────────╮
 │ strlen │ capacity │ FixedSize│FixedSize│no limit │         │         │         │
 │ (chars)│ (strings)│ stringBuf│std:queue│std:queue│ (1)/(1) │ (2)/(1) │ (3)/(1) │
 │        │          │    (1)   │   (2)   │  (3)    │         │         │         │
 ├────────┼──────────┼──────────┼─────────┼─────────┼─────────┼─────────┼─────────┤
 │      1 │     10.3 │      5ns │     8ns │     4ns │    1.0X │    1.6X │    0.8X │
 │     10 │     10.3 │      7ns │     8ns │     5ns │    1.0X │    1.1X │    0.7X │
 │    100 │     10.3 │      7ns │    28ns │    43ns │    1.0X │    4.0X │    6.1X │
 │   1000 │     10.3 │     17ns │    71ns │   200ns │    1.0X │    4.2X │   11.8X │
 ╰────────┴──────────┴──────────┴─────────┴─────────┴─────────┴─────────┴─────────╯
```

This run shows a 4X speedup compared with a std::queue based ring buffer for strings longer than 100 characters on
average. As strings become longer, the overhead of using unbounded string std::queue becomes significant:
more than 10X slower for strings longer than 1000 characters. Ring buffer wins over unbounded 
queues because it avoids the extra memory allocation time.

## Getting Started

`make help` gives the following options:

```bash
help                 this message
clean                remove build dir
debug                create slow debug version
release              create optimized release version
test                 run tests under test/ dir
bench                run benchmark under bench/ dir
coverage             check code coverage
docs                 generate Doxygen HTML documentation, including API docs
install              install the package to the INSTALL_LOCATION=~/.local
format               format the project sources
```

`make release` builds the example [src/main.c](src/main.c) .

An trivial example looks like:

```cpp
#include "fixed_size_string_buffer.h"
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
#include "fixed_size_string_buffer.h"

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

See example1 in [src/main.cpp](src/main.cpp) for a similar demo.
Run `make docs` to see doxygen version of class descriptions.

### Coverage

`make coverage` generates coverage information that should sometime like:

```bash
LCOV - code coverage report

Current view: top level                       Hit     Total   Coverage
Test: coverage.info           Lines:          403     403     100.0 %
Date: 2023-04-01 17:36:13     Functions:      117     118     99.2 %

Filename                           Line Coverage            Function
fixed_size_queue.h	           100.0%   100.0 %	 22 /  22	100.0 %	 6 /  6
fixed_size_string_buffer.h	   100.0%   100.0 %	196 / 196	 98.7 %	75 / 76

Generated by: LCOV version 1.16
```

### Tests and Benchmark

`make test` invokes google test on [test/test_basic.cpp](test/test_basic.cpp) and [test/test_compare.cpp](test/test_compare.cpp)
Expected results look something like:
```bash
1: [==========] Running 5 tests from 2 test suites.
1: [----------] Global test environment set-up.
1: [----------] 3 tests from QueueTest
1: [ RUN      ] QueueTest.Small
1: [       OK ] QueueTest.Small (0 ms)
1: [ RUN      ] QueueTest.Warnings
1: [       OK ] QueueTest.Warnings (0 ms)
1: [ RUN      ] QueueTest.Large
1: [       OK ] QueueTest.Large (0 ms)
1: [----------] 3 tests from QueueTest (0 ms total)
1:
1: [----------] 2 tests from Compare
1: [ RUN      ] Compare.WithFixedQueue
1: [       OK ] Compare.WithFixedQueue (0 ms)
1: [ RUN      ] Compare.WithUnlimitedQueue
1: [       OK ] Compare.WithUnlimitedQueue (0 ms)
1: [----------] 2 tests from Compare (0 ms total)
1:
1: [----------] Global test environment tear-down
1: [==========] 5 tests from 2 test suites ran. (1 ms total)
1: [  PASSED  ] 5 tests.
```

`make bench` runs trials on push operation under varying conditions of string length, eg:

```bash
Running ./unit_bench
Run on (10 X 24.1205 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB
  L1 Instruction 128 KiB
  L2 Unified 4096 KiB (x10)
Load Average: 4.59, 4.82, 5.12
----------------------------------------------------------------------------------------
Benchmark                                              Time             CPU   Iterations
----------------------------------------------------------------------------------------
BM_queue<QueueType::FixedBuffer, 1, 10, 3>          4.76 ns         4.75 ns    135930248
BM_queue<QueueType::FixedQueue, 1, 10, 3>           7.34 ns         7.33 ns     94723880
BM_queue<QueueType::StdQueue, 1, 10, 3>             5.86 ns         4.70 ns    144807613
BM_queue<QueueType::FixedBuffer, 10, 10, 3>         5.40 ns         5.40 ns    128158184
BM_queue<QueueType::FixedQueue, 10, 10, 3>          7.31 ns         7.30 ns     96702447
BM_queue<QueueType::StdQueue, 10, 10, 3>            6.44 ns         4.66 ns    152621159
BM_queue<QueueType::FixedBuffer, 100, 10, 3>        5.56 ns         5.56 ns    125362656
BM_queue<QueueType::FixedQueue, 100, 10, 3>         27.4 ns         27.4 ns     25891883
BM_queue<QueueType::StdQueue, 100, 10, 3>           40.6 ns         37.8 ns     23485913
BM_queue<QueueType::FixedBuffer, 1000, 10, 3>       16.1 ns         16.1 ns     43470701
BM_queue<QueueType::FixedQueue, 1000, 10, 3>        71.0 ns         70.9 ns      9662102
BM_queue<QueueType::StdQueue, 1000, 10, 3>           181 ns          132 ns      5148308
``` 

The last line for example is running a case where the string added to queue is 1000 chars, 
while the queue size is 10.3 * string length = 10300 chars.  
It takes 132ns per push using an unbounded `std::queue<std::string>`, which reduces to
70.9 ns with the `std::queue` of fixed char length from [fixed_size_queue.h](include/fixed_size_queue.h).
Using ring buffer with this large string length drops the push time to 16.1 ns.

### Porting

Linux and MacOS build fine, see [https://github.com/d-e-e-p/fixed-size-string-buffer/actions] (https://github.com/d-e-e-p/fixed-size-string-buffer/actions)

On windows, unicode in the source files make build a bit fragile.
For MS Visual Code to handle unicode correctly on Windows, we need 4 steps:

1. Terminal output to support UTF8 (with `chcp.com 65001`)
2. Source files encoded UTF-8 with BOM
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
