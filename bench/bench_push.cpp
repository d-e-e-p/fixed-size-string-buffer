
#include <array>
#include <queue>
#include <benchmark/benchmark.h>

#include "fixed_size_string_buffer.h"
#include "fixed_size_queue.h"

enum class QueueType {FixedBuffer, FixedQueue, StdQueue};


template <QueueType qtype, size_t LEN, size_t CAPACITY, size_t EXCESS>
void BM_queue(benchmark::State& state)
{

  // create buffer just enough size to hold str_capacity_in_buffer values
  auto str_test = std::string(LEN, 'x');
  constexpr double str_capacity_in_buffer = CAPACITY + 0.1 * EXCESS;
  constexpr auto max_size = static_cast<size_t>(LEN * str_capacity_in_buffer);


  switch (qtype) {
    case QueueType::FixedBuffer: {
      auto queue = FixedSizeStringBuffer<LEN>();
      for (auto _ : state) { queue.push(str_test); }
      break;
    }
    case QueueType::FixedQueue: { 
        auto queue = FixedQueue(max_size);
        for (auto _ : state) { queue.push(str_test); }
        break;
    }
    case QueueType::StdQueue: {
        std::queue<std::string> queue = {};
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
BENCHMARK_TEMPLATE(BM_queue, QueueType::FixedBuffer,  len, capacity, excess); \
BENCHMARK_TEMPLATE(BM_queue, QueueType::FixedQueue,   len, capacity, excess); \
BENCHMARK_TEMPLATE(BM_queue, QueueType::StdQueue,     len, capacity, excess);

// Len Capacity Excess
gen(    1, 10, 3)
gen(   10, 10, 3)
gen(  100, 10, 3)
gen( 1000, 10, 3)


BENCHMARK_MAIN();

