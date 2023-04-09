
#
# from https://www.danielsieger.com/blog/2022/03/06/code-coverage-for-cpp.html
#

if(ENABLE_COVERAGE)
  # set compiler flags
  set(CMAKE_CXX_FLAGS "-O0 -coverage")

  # find required tools
  #find_package(lcov)
   if(NOT lcov_FOUND)
        #CPMAddPackage("https://github.com/linux-test-project/lcov/archive/refs/tags/v1.16.zip")
        CPMAddPackage("gh:linux-test-project/lcov@1.16")
        set(LCOV lcov)
        set(GENHTML genhtml)
   endif()

   find_program(genhtml QUIET)

  # add coverage target
  add_custom_target(coverage
    # gather data
    COMMAND ${LCOV} --directory CMakeFiles --capture --output-file coverage_all.info 
    COMMAND ${LCOV} --remove coverage_all.info '*/usr/include/*' '*/googletest/*' --output-file coverage.info
    # generate report
    COMMAND ${GENHTML} --demangle-cpp -o coverage coverage.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

