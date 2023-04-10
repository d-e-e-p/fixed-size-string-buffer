
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

See [fixed_elem_size_queue.h](include/fssb/fixed_elem_size_queue.h) or [boost::circular_buffer](https://www.boost.org/doc/libs/1_61_0/doc/html/boost/circular_buffer.html).  
You could extend this to take into account the sizes of strings added, eg
[fixed_char_size_queue.h](include/fssb/fixed_char_size_queue.h)
This achieves pretty much the same functionality of this library except the requirement to 
pre-allocate buffer space at compile time.

Turns out that pre-allocation can sometimes be very useful.  For example, in embeded devices 
we need very predictable dynamic allocation limits to maintain sufficient memory headroom.  
This ring buffer array can be allocated statically so it ends up in the [.bss section](https://en.wikipedia.org/wiki/.bss), 
when can then be alloted to a dedicated bank (eg CCM Memory on a STM32). This eliminates the possibility
of conflict between message buffer and operating heap/stack memory.  See writeup on [Using CCM
Memory](https://www.openstm32.org/Using%2BCCM%2BMemory). 

There is also a significant speed advantage of using this approach for long strings, eg on macos:

```bash
         fixed_size_string_buffer :
   wallclock time comparison for push operation
 ╭────────┬──────────┬──────────┬──────────┬────────╮
 │ strlen │ max_size │ FixedSize│ FixedChar│ RATIO  │
 │ (chars)│  (chars) │ stringBuf│ std:queue│        │
 │        │          │    (1)   │    (2)   │(2)/(1) │
 ├────────┼──────────┼──────────┼──────────┼────────┤
 │     10 │      103 │      6ns │      6ns │   1.0X │
 │    100 │     1030 │      7ns │     25ns │   3.6X │
 │   1000 │    10300 │     18ns │     70ns │   3.9X │
 ╰────────┴──────────┴──────────┴──────────┴────────╯
     (1)  FixedSizeStringBuffer<max_size>()
     (2)  FixedCharSizeQueue(max_size)

        max_size = (10.3 * strlen) characters
```

This example run shows a 3.6X speedup for strings longer than 100 characters, compared with a std::queue<string> based approach.
As strings become longer, the overhead of using std::queue becomes even more pronounced.
Ring buffer wins over unbounded queues because it avoids the extra memory allocation time.

## Getting Started

`make help` gives the following options:

```bash
help                 this message
clean                remove build dir
debug                create debug version of standalone examples
release              create optimized version of examples
bench                run benchmark on push operation
test                 exercise all queue operations
coverage             check code coverage
install              copy include files to install location
docs                 generate Doxygen HTML documentation, including API docs
windows_unicode_fix  needed for unicode output on windows
```

`make -j4 all` will run the following steps: 
```bash
all: clean debug release test bench coverage install docs
```

`make release` builds the examples under [standalone/source](standalone/source/)
A trivial usage looks like:

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

demo can be compiled with:

```bash
g++ -std=c++17 -I include test.cpp
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
This does not affect benchmark measurements, only the metadata output.
2023-04-08T05:35:38-04:00
Running ./build/bench/unit_bench
Run on (10 X 24.0056 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB
  L1 Instruction 128 KiB
  L2 Unified 4096 KiB (x10)
Load Average: 12.13, 16.52, 19.52
-----------------------------------------------------------------------------------------------
Benchmark                                                     Time             CPU   Iterations
-----------------------------------------------------------------------------------------------
BM_queue<QType::BoostCircularBuffer, 10, 10, 3>             584 ns          477 ns      1541714
BM_queue<QType::FixedSizeStringBuffer , 10, 10, 3>         2196 ns         2191 ns       312897
BM_queue<QType::FixedCharSizeQueue, 10, 10, 3>             2013 ns         2008 ns       338128
BM_queue<QType::FixedElemSizeQueue, 10, 10, 3>             2122 ns         2114 ns       337221
BM_queue<QType::StdQueue, 10, 10, 3>                       1605 ns         1439 ns       484600
BM_queue<QType::BoostCircularBuffer, 100, 10, 3>           1485 ns         1484 ns       473165
BM_queue<QType::FixedSizeStringBuffer , 100, 10, 3>        2345 ns         2344 ns       294500
BM_queue<QType::FixedCharSizeQueue, 100, 10, 3>            6942 ns         6931 ns       102775
BM_queue<QType::FixedElemSizeQueue, 100, 10, 3>           13726 ns        13706 ns        50842
BM_queue<QType::StdQueue, 100, 10, 3>                     10638 ns         9005 ns        89370
BM_queue<QType::BoostCircularBuffer, 1000, 10, 3>          4837 ns         4794 ns       151938
BM_queue<QType::FixedSizeStringBuffer , 1000, 10, 3>       5786 ns         5754 ns       117102
BM_queue<QType::FixedCharSizeQueue, 1000, 10, 3>          19138 ns        19110 ns        36170
BM_queue<QType::FixedElemSizeQueue, 1000, 10, 3>          38308 ns        38265 ns        18095
BM_queue<QType::StdQueue, 1000, 10, 3>                    65654 ns        43790 ns        16187
``` 

The `BM_queue<QType::BoostCircularBuffer, 1000, 10, 3>` line is running boost circular::buffer
with strings added to queue average 1000 chars each, and queue limit is 10 strings.

FixedSizeStringBuffer and FixedCharSizeQueue have character limits instead of string limits:
with `1000, 10, 3>` the limit is 1000 * 10.3 = 10300 chars, with `100, 10, 3>` the buffer limit
is 100 * 10.3 = 1030 characters.

Bottom line: if you know the string lengths up front then the fastest approach is BoostCircularBuffer.
If strings lengths are variable and there are advantages to pre-allocation of memory, FixedSizeStringBuffer
gives similar speeds for push operation.


### Porting

Linux and MacOS build fine, see [https://github.com/d-e-e-p/fixed-size-string-buffer/actions](https://github.com/d-e-e-p/fixed-size-string-buffer/actions)

On windows, compiling wth unicode in the source files is a bit fragile.
For MS Visual Code to handle unicode correctly on windows, we need 3 steps:

1. Update terminal codepage for output to support UTF8 (with `chcp.com 65001`)
2. `#pragma execution_character_set("utf-8")` in include files
3. Compile with `/utf-8` option
<s>4. Encode source files as UTF-8 *with* BOM</s>

See section on `if: runner.os == 'Windows'` in the action config 
[.github/workflows/ci.yml](.github/workflows/ci.yml). 

![ci workflow status](https://github.com/d-e-e-p/fixed-size-string-buffer/actions/workflows/ci.yml/badge.svg)

### Dir Structure
There's no top level cmake--I find it cleaner to have each target pretend to be top level and build in a separate build dir.
So `make release` runs under `build/release`, while make test runs under `build/test`.  Running `make -j4 all` runs all steps.

```bash
Makefile                [drive all the steps]
   
├── include             [source files]
│   └── fssb
│       ├── fixed_char_size_queue.h
│       ├── fixed_elem_size_queue.h
│       └── fixed_size_string_buffer.h


├── standalone          [examples of operating the queue]
│   ├── CMakeLists.txt
│   └── source
│       ├── basic_example.cpp
│       ├── demo.cpp
│       └── unicode_example.cpp

├── test                [exercise all aspects of ring buffer]
│   ├── CMakeLists.txt
│   └── source
│       ├── run_all.cpp
│       ├── test_basic.cpp
│       └── test_compare.cpp


├── bench               [compare runtime between alternatives]
│   ├── CMakeLists.txt
│   └── source
│       └── bench_push.cpp


├── cmake               [cmake helper routines]
│   ├── CPM.cmake
│       ...

├── docs                [generation of static documentation site]
│   ├── CMakeLists.txt
│   ├── Doxyfile
│   ├── conf.py
│   └── pages
│       └── about.dox


├── install             [install collateral and setup]
│   └── CMakeLists.txt


├── scripts             [random scripts]
│   └── remove_actions.bash
```

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

This project follows [SemVer](http://semver.org/) conventions for versioning. 
A list of existing versions can be found in the
[project's releases](https://github.com/d-e-e-p/fixed-size-string-buffer/releases).

## Authors

* **Sandeep** - [@d-e-e-p](https://github.com/d-e-e-p)

## License

The project is available under the [MIT](https://opensource.org/licenses/MIT) license.
See [LICENSE](LICENSE) file for details
