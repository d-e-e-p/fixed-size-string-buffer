
#
# from https://www.danielsieger.com/blog/2022/03/06/code-coverage-for-cpp.html
#

if(ENABLE_COVERAGE)
  # set compiler flags
  set(CMAKE_CXX_FLAGS "-O0 -coverage")

  # find required tools
  find_program(LCOV lcov REQUIRED)
  find_program(GENHTML genhtml REQUIRED)

  # add coverage target
  add_custom_target(coverage
    # gather data
    COMMAND ${LCOV} --directory . --capture --output-file coverage_all.info 
    COMMAND ${LCOV} --directory . --remove coverage_all.info '*/usr/include/*' '*/_deps/gtest-src/*/*' '*/_deps/gtest-src/*/*/*' --output-file coverage.info
    # generate report
    COMMAND ${GENHTML} --demangle-cpp -o coverage coverage.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

