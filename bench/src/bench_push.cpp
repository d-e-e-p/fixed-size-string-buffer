
/**
 * @file  bench_push.cpp
 *
 * @section DESCRIPTION
 * benchmark speed of push operations on various fixed size queue options
 *
 * https://github.com/d-e-e-p/fixed-size-string-buffer
 * Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
 *
 * @section LICENSE
 *
 * MIT License <http://opensource.org/licenses/MIT>
 *
 */

#include <string>
#include <array>
#include <queue>
#include <any>

#include <benchmark/benchmark.h>
#include <boost/circular_buffer.hpp>

#include "fssb/fixed_size_string_buffer.h"
#include "fssb/fixed_elem_size_queue.h"
#include "fssb/fixed_char_size_queue.h"

enum class QType {FixedSizeStringBuffer, FixedCharSizeQueue, FixedElemSizeQueue, BoostCircularBuffer, StdQueue};


// wraper to alias push -> push_back
class WrapperBoostCircularBuffer : public boost::circular_buffer<std::string>
{
public:
  explicit WrapperBoostCircularBuffer(size_t max): boost::circular_buffer<std::string>(max) 
  {
  }
  void push(const std::string& value) 
  {
    this->push_back(value);
  }
};


#define QUEUE_TEST \
      for (auto _ : state) { \
        for (size_t i = 0; i < num_iter; ++i) { \
          queue.push(str_long); \
          queue.push(str_med); \
          queue.push(str_short); \
        } \
      } \




template <QType qtype, size_t LEN, size_t CAPACITY, size_t EXCESS>
void BM_queue(benchmark::State& state)
{

  // create str of 3 different sizes
  constexpr double scaling_factor = 1.2;
  auto scale_long  = static_cast<size_t>(static_cast<double>(LEN) * scaling_factor);
  auto scale_med   = LEN;
  auto scale_short = static_cast<size_t>(static_cast<double>(LEN) / scaling_factor);

  auto str_long   = std::string(scale_long,  'l');
  auto str_med    = std::string(scale_med,   'm');
  auto str_short  = std::string(scale_short, 's');

  constexpr double str_capacity_in_buffer = CAPACITY + 0.1 * EXCESS;
  constexpr auto max_size = static_cast<size_t>(LEN * str_capacity_in_buffer);
  constexpr size_t num_iter = 100;

  switch (qtype) {
    case QType::FixedSizeStringBuffer: {
      auto queue = fssb::FixedSizeStringBuffer<max_size>();
      QUEUE_TEST
      break;
    }
    case QType::FixedCharSizeQueue: { 
      auto queue = fssb::FixedCharSizeQueue(max_size);
      QUEUE_TEST
      break;
    }
    case QType::FixedElemSizeQueue: {
      auto queue = fssb::FixedElemSizeQueue<std::string>(CAPACITY);
      QUEUE_TEST
      break;
    }
    case QType::BoostCircularBuffer: {
      auto queue = WrapperBoostCircularBuffer(CAPACITY);
      QUEUE_TEST
      break;
    }
    case QType::StdQueue: {
      auto queue = std::queue<std::string>();
      QUEUE_TEST
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
BENCHMARK_TEMPLATE(BM_queue, QType::BoostCircularBuffer,    len, capacity, excess); \
BENCHMARK_TEMPLATE(BM_queue, QType::FixedSizeStringBuffer , len, capacity, excess); \
BENCHMARK_TEMPLATE(BM_queue, QType::FixedCharSizeQueue,     len, capacity, excess); \
BENCHMARK_TEMPLATE(BM_queue, QType::FixedElemSizeQueue,     len, capacity, excess); \
BENCHMARK_TEMPLATE(BM_queue, QType::StdQueue,               len, capacity, excess); \


// Len Capacity Excess
gen(   10, 10, 3)
gen(  100, 10, 3)
gen( 1000, 10, 3)


BENCHMARK_MAIN();

