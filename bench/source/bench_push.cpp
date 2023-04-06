
#include <array>
#include <queue>
#include <benchmark/benchmark.h>

#include "fssb/fixed_size_string_buffer.h"
#include "fssb/fixed_elem_size_queue.h"
#include "fssb/fixed_char_size_queue.h"

enum class QType {FixedSizeStringBuffer, FixedCharSizeQueue, FixedElemSizeQueue, StdQueue};


template <QType qtype, size_t LEN, size_t CAPACITY, size_t EXCESS>
void BM_queue(benchmark::State& state)
{

  // create buffer just enough size to hold str_capacity_in_buffer values
  auto str_test = std::string(LEN, 'x');
  constexpr double str_capacity_in_buffer = CAPACITY + 0.1 * EXCESS;
  constexpr auto max_size = static_cast<size_t>(LEN * str_capacity_in_buffer);


  switch (qtype) {
    case QType::FixedSizeStringBuffer: {
      auto queue = FixedSizeStringBuffer<LEN>();
      for (auto _ : state) { queue.push(str_test); }
      break;
    }
    case QType::FixedCharSizeQueue: { 
        auto queue = FixedCharSizeQueue(max_size);
        for (auto _ : state) { queue.push(str_test); }
        break;
    }
    case QType::FixedElemSizeQueue: {
        auto queue = FixedElemSizeQueue<std::string>(CAPACITY);
        for (auto _ : state) { queue.push(str_test); }
        break;
    }
    case QType::StdQueue: {
        auto queue = std::queue<std::string>();
        for (auto _ : state) { queue.push(str_test); }
        break;
    }
  }

}

/*
constexpr std::array<size_t, 5> kLen  = {10, 100, 200, 500, 1000};
constexpr size_t kCapacity = 10;
constexpr size_t kExcess = 3;
*/

#define gen(len, capacity, excess) \
BENCHMARK_TEMPLATE(BM_queue, QType::FixedSizeStringBuffer , len, capacity, excess); \
BENCHMARK_TEMPLATE(BM_queue, QType::FixedCharSizeQueue,     len, capacity, excess); \
BENCHMARK_TEMPLATE(BM_queue, QType::FixedElemSizeQueue,     len, capacity, excess); \
BENCHMARK_TEMPLATE(BM_queue, QType::StdQueue,               len, capacity, excess);

// Len Capacity Excess
gen(   10, 10, 3)
gen(  100, 10, 3)
gen( 1000, 10, 3)


BENCHMARK_MAIN();

