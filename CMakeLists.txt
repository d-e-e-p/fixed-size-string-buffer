cmake_minimum_required(VERSION 3.14)

project(
  fixed_size_string_buffer
  VERSION 1.1.0
  LANGUAGES CXX
  DESCRIPTION "A C++ pre-allocated ring buffer for std::string messages"
  HOMEPAGE_URL "https://github.com/d-e-e-p/fixed-size-string-buffer"
)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)

