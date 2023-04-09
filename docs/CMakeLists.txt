cmake_minimum_required(VERSION 3.14...3.22)

project(
  "fixed_size_string_buffer"
  VERSION 1.1.0
  LANGUAGES CXX
  DESCRIPTION "A C++ pre-allocated ring buffer for std::string messages"
  HOMEPAGE_URL "https://github.com/d-e-e-p/fixed-size-string-buffer/wiki"
)

# ---- Dependencies ----

include(../cmake/common.cmake)

CPMAddPackage("gh:mosra/m.css#ea904dbb82b6e0d10ac2cf319ca065b4cc869ab3")
#CPMAddPackage(NAME ${PROJECT_NAME} SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../include)
#find_package(Doxygen QUIET)
if(NOT Doxygen_FOUND)
      CPMAddPackage("https://github.com/doxygen/doxygen/archive/refs/tags/Release_1_9_6.zip")
      #CPMAddPackage("gh:linux-test-project/lcov@1.16")
      set(LCOV "${lcov_SOURCE_DIR}/bin/lcov")
      set(GENHTML "${lcov_SOURCE_DIR}/bin/genhtml")
endif()


# ---- Doxygen variables ----

# set Doxyfile variables
set(DOXYGEN_PROJECT_NAME ${PROJECT_NAME})
set(DOXYGEN_PROJECT_VERSION ${PROJECT_VERSION})
set(DOXYGEN_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doxygen")

set(PYTHONOPTIMIZE 1)

configure_file(${CMAKE_CURRENT_LIST_DIR}/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

configure_file(${CMAKE_CURRENT_LIST_DIR}/conf.py ${CMAKE_CURRENT_BINARY_DIR}/conf.py)

add_custom_target(
  GenerateDocs
  ${CMAKE_COMMAND} -E make_directory "${DOXYGEN_OUTPUT_DIRECTORY}"
  COMMAND python3 -O "${m.css_SOURCE_DIR}/documentation/doxygen.py" "${CMAKE_CURRENT_BINARY_DIR}/conf.py"
  COMMAND echo "Docs written to: ${DOXYGEN_OUTPUT_DIRECTORY}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
