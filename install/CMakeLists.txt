cmake_minimum_required(VERSION 3.15...3.22)

project(
  "fixed_size_string_buffer"
  VERSION 1.1.0
  LANGUAGES CXX
  DESCRIPTION "A C++ pre-allocated ring buffer for std::string messages"
  HOMEPAGE_URL "https://github.com/d-e-e-p/fixed-size-string-buffer/wiki"
)

include(../cmake/common.cmake)

# ---- Create install package ----- ----
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE  
    $<INSTALL_INTERFACE:../include>  # <prefix>/include/mylib
)

CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.10.0")

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  NAMESPACE ${PROJECT_NAME}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../include
  INCLUDE_DESTINATION include
)

