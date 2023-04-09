cmake_minimum_required(VERSION 3.15...3.22)

project(unit_tests LANGUAGES CXX)
enable_testing()

# --- Import tools ----
include(../cmake/common.cmake)

# ---Setup GoogleTest --

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(CMAKE_BUILD_TYPE "Debug")

#FetchContent_MakeAvailable(googletest-src)
CPMAddPackage("gh:google/googletest@1.13.0")

# ---Config Src --------

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

message(STATUS "sources = ${sources} dir=${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(${PROJECT_NAME} ${sources})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_NAME}
  gtest_main
  gmock_main
)
include_directories("../include")

add_test(
  NAME tests
  COMMAND ${PROJECT_NAME}
)
