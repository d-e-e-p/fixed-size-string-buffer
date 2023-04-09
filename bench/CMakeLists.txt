
cmake_minimum_required(VERSION 3.14...3.22)

project(unit_bench LANGUAGES CXX)

# ---- Dependencies ----
include(../cmake/common.cmake)

# ---- Settings --------
set(BENCHMARK_ENABLE_TESTING OFF)
set(CMAKE_BUILD_TYPE "Release")

# ---- Packages --------
find_package(Boost QUIET)
if(NOT Boost_FOUND)
    CPMAddPackage("https://github.com/boostorg/boost/releases/download/boost-1.81.0/boost-1.81.0.zip")
    find_all_include_directories(${boost_SOURCE_DIR} Boost_INCLUDE_DIRS)
endif()

#  URL https://github.com/google/benchmark/archive/refs/tags/v1.7.1.zip
CPMAddPackage("gh:google/benchmark@1.7.1")

# ---- Targets ---------
file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable(${PROJECT_NAME} ${sources})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_NAME} benchmark)

target_include_directories(${PROJECT_NAME} PRIVATE "../include")
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

#dump_all_vars()
