include(FetchContent)
FetchContent_Declare(gbench
  QUIET
  DOWNLOAD_EXTRACT_TIMESTAMP true
  URL https://github.com/google/benchmark/archive/refs/tags/v1.7.1.zip
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
FetchContent_MakeAvailable(gbench)

set(bench_src
  bench_push.cpp
)

add_executable(unit_bench ${bench_src})
target_compile_features(unit_bench PRIVATE cxx_std_17)
# execute after build
add_custom_command(TARGET unit_bench POST_BUILD
    COMMAND $<TARGET_FILE:unit_bench>
)


target_link_libraries(unit_bench
  benchmark
)

include_directories("../include")
set_target_properties( unit_bench benchmark benchmark_main
    PROPERTIES
    CXX_CPPCHECK ""
    CXX_CLANG_TIDY ""
)
