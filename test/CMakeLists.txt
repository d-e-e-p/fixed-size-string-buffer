include(FetchContent)
FetchContent_Declare(googletest-src 
  QUIET
  DOWNLOAD_EXTRACT_TIMESTAMP true
  URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest-src)

set(test_src
  run_all.cpp
  test_basic.cpp  
  test_compare.cpp
)

add_executable(unit_tests ${test_src})
target_compile_features(unit_tests PRIVATE cxx_std_17)

target_link_libraries(unit_tests
  gtest_main
  gmock_main
)
include_directories("../include")
set_target_properties( gtest gtest_main unit_tests
    PROPERTIES
    CXX_CPPCHECK ""
    CXX_CLANG_TIDY ""
)

add_test(
  NAME tests
  COMMAND unit_tests
)
