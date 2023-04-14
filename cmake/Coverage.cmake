
#
# from https://www.danielsieger.com/blog/2022/03/06/code-coverage-for-cpp.html
#


function(enable_coverage)
  # set compiler flags
  if(MSVC)
    # SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/PROFILE")
    set(CMAKE_CXX_FLAGS "/PROFILE" PARENT_SCOPE)
  else()
      set(CMAKE_CXX_FLAGS "-O0 -coverage -fprofile-arcs -ftest-coverage" PARENT_SCOPE)
  endif()

  # find required tools
  find_package(lcov QUIET)
  find_package(genhtml QUIET)
  if(NOT lcov_FOUND)
        #CPMAddPackage("https://github.com/linux-test-project/lcov/archive/refs/tags/v1.16.zip")
        CPMAddPackage("gh:linux-test-project/lcov@1.16")
        set(LCOV "${lcov_SOURCE_DIR}/bin/lcov")
        set(GENHTML "${lcov_SOURCE_DIR}/bin/genhtml")
  endif()


  # add coverage target
  add_custom_target(coverage
    # gather data
    COMMAND ${LCOV} --directory CMakeFiles --capture --output-file coverage_all.info 
    COMMAND ${LCOV} --remove coverage_all.info '*/usr/include/*' '*/catch2/*' '*/googletest/*' --output-file coverage.info
    # generate report
    COMMAND ${GENHTML} --demangle-cpp -o coverage coverage.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

endfunction()

