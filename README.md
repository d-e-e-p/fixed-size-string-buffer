
# A pre-allocated ring buffer for std::string messages

A C++ char buffer sized at compile-time that stores string messages in a ring buffer.
When the buffer is full, oldest strings are deleted to make way for the new entries.
Requires at least -std=c++14 .

## Credits

- Project template from [modern-cpp-template](https://github.com/filipdutescu/modern-cpp-template)
- Single file header and other setting from [p-ranav](https://github.com/p-ranav)
- Discussion at stackexchange [Elegant Circular Buffer](https://codereview.stackexchange.com/questions/164130/elegant-circular-buffer)

## Motivation

Some applications need a fixed-size [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer)
with automatic overwrite of tail messages by the head.  
If the sizes of strings were known, then one way to achive this would be to 
budget for a target capacity and then pop-on-push like:
```cpp
std::deque<std::string> q;
...
if (q.size() > target_capacity) {
   q.pop_front();
 }
 q.push_back(new_string);
```

Of course, it would be easier to wrap all this into a class that takes care
of pop-on-push opperation behind the scenes. See this [gist](https://gist.github.com/d-e-e-p/fc2697bdef0faa11678fe034d44772d3) .
You could extend this to take into account the sizes of strings added, and 
achieve pretty much the same functionality of this library except the ability to pre-allocate 
buffer space on the stack at compile time.

Turns out that's sometimes pretty useful.  For embeded devices for example we need to limit 
dynamic allocation and maintain plenty of ram headroom.  This char buffer array can be 
allocated statically to make it part of the [.bss section](https://en.wikipedia.org/wiki/.bss), 
when can be alloted to a dedicated bank (eg CCM Memory). This eliminates the possibility
of conflict between message buffer and operating heap/stack memory.  See writeup on [Using CCM
Memory](https://www.openstm32.org/Using%2BCCM%2BMemory).

There is also a speed advantage of using this approach:

```bash
make test
./build/bin/Release/fixed_size_string_buffer

average FixedSizeStringBuffer push time is 23ns
average std::deque push time is 71ns
  => speedup is 3.08696 X
```

## Getting Started

`make help` gives the following options:

```bash
help                 this message
test                 run tests quickly with ctest
coverage             check code coverage quickly GCC
docs                 generate Doxygen HTML documentation, including API docs
install              install the package to the INSTALL_LOCATION ~/.local
format               format the project sources
```

`make install` builds the example `src/main.c` 

An trivial example looks like:

```cpp
#include "fixed_size_string_buffer.hpp"
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
g++ -std=c++14 -I include test.cpp
./a.out
```

## API

Externally this class looks like a simple queue with infinite space.

```cpp
// Constructor
constexpr size_t max_size = 12;
auto rb = FixedSizeStringBuffer<max_size>();

// Modifiers
fsb.push("abc");     // inserts element at the end
fsb.pop();           // removes the first element

// Element access

fsb.front();        // access the first element
fsb.back();         // access the last element
fsb.size();         // number of strings in buffer
fsb.empty();        // is the underlying container is empty?
fsb.free_space();   // unused character space in buffer
fsb.at(0);          // string at location=0 (front)

// Debug
fsb.set_debug(true); // turns on debug messages
std::cout << fsb;    // pretty prints the buffer status
```

In this example, three messages stuffed into a buffer that can 
only hold two:

```cpp
#include <iostream>
#include "fixed_size_string_buffer.hpp"

int main() {
  auto foo = FixedSizeStringBuffer<8>();

  // push strings into buffer
  foo.push("0aa");
  foo.push("1bb");
  foo.push("2cc");  // 0aa gets pushed out to make room for 2cc
                    // foo only has space for 8 chars 
  // inspect  buffer
  std::cout << foo << "\n";
  std::cout << "foo.front(): " << foo.front()  << "\n";
  std::cout << "foo.back() : " << foo.back()   << "\n";
  std::cout << "foo[0]     : " << foo[0]       << "\n";
  std::cout << "foo.pop()  : " << foo.pop()    << "\n";
  std::cout << "foo[0]     : " << foo[0]       << "\n";
  return 0;
}
```

the result of running this should look like:

```bash
 chars[ 0] = c
 chars[ 1] = a <-- back
 chars[ 2] = a
 chars[ 3] = 1 <-- ptr[ 0] <-- front
 chars[ 4] = b
 chars[ 5] = b
 chars[ 6] = 2 <-- ptr[ 1]
 chars[ 7] = c

   ptr[ 0] = 3 str = 1bb
   ptr[ 1] = 6 str = 2cc

foo.front(): 1bb
foo.back() : 2cc
foo[0]     : 1bb
foo.pop()  : 1bb
foo[0]     : 2cc
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

This project makes use of [SemVer](http://semver.org/) for versioning. A list of
existing versions can be found in the
[project's releases](https://github.com/d-e-e-p/fixed-size-string-buffer/releases).

## Authors

* **Sandeep** - [@d-e-e-p](https://github.com/d-e-e-p)

## License

The project is available under the [MIT](https://opensource.org/licenses/MIT) license.
See [LICENSE](LICENSE) file for details
