# docs cmake
cmake_minimum_required(VERSION 3.14...3.22)
include(../cmake/common.cmake)

# ---- Setup project -------
project(${PROJECT} VERSION ${VERSION})

# ---- Dependencies ----
CPMAddPackage("gh:mosra/m.css#ea904dbb82b6e0d10ac2cf319ca065b4cc869ab3")
find_package(Doxygen REQUIRED)


# ---- Doxygen variables ----
set(DOXYGEN_PROJECT_NAME ${PROJECT_NAME})
set(DOXYGEN_PROJECT_VERSION ${PROJECT_VERSION})
set(DOXYGEN_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doxygen")

set(PYTHONOPTIMIZE 1) # avoid debug errors from stopping doxygen.py

configure_file(${CMAKE_CURRENT_LIST_DIR}/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
configure_file(${CMAKE_CURRENT_LIST_DIR}/conf.py ${CMAKE_CURRENT_BINARY_DIR}/conf.py)

add_custom_target(
  GenerateDocs
  ${CMAKE_COMMAND} -E make_directory "${DOXYGEN_OUTPUT_DIRECTORY}"
  COMMAND python3 -O "${m.css_SOURCE_DIR}/documentation/doxygen.py" "${CMAKE_CURRENT_BINARY_DIR}/conf.py"
  COMMAND echo "Docs written to: ${DOXYGEN_OUTPUT_DIRECTORY}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
