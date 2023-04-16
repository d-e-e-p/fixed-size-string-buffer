# install cmake
cmake_minimum_required(VERSION 3.15...3.22)

project(
  fixed-size-string-buffer
  VERSION 1.4.0
  LANGUAGES CXX
  DESCRIPTION "A C++ pre-allocated ring buffer for std::string messages"
  HOMEPAGE_URL "https://github.com/d-e-e-p/fixed-size-string-buffer/wiki"
)

include(../cmake/common.cmake)

# ---- Create install package ----- ----
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE  
    $<INSTALL_INTERFACE:include>  
)
include(CMakePrintHelpers)
cmake_print_properties(TARGETS ${PROJECT_NAME} PROPERTIES NAME TYPE INTERFACE_INCLUDE_DIRECTORIES)

CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.10.0")
packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  NAMESPACE ${PROJECT_NAME}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../include
  INCLUDE_DESTINATION include
)

