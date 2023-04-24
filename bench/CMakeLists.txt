# bench cmake 

cmake_minimum_required(VERSION 3.14...3.22)
include(../cmake/common.cmake)

# ---- Setup Project----
project(unit_bench LANGUAGES CXX)
common_compiler_settings()

# ---- Settings --------
set(CMAKE_BUILD_TYPE "Release")
set(BENCHMARK_ENABLE_TESTING OFF)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# ---- Packages --------
CPMAddPackage(
  NAME Boost
  GITHUB_REPOSITORY "boostorg/boost"
  GIT_TAG "boost-1.82.0"
  DOWNLOAD_ONLY
)
cmake_print_properties(TARGETS Boost::circular_buffer PROPERTIES NAME TYPE INTERFACE_INCLUDE_DIRECTORIES)

#  URL https://github.com/google/benchmark/archive/refs/tags/v1.7.1.zip
CPMAddPackage("gh:google/benchmark@1.7.1")

# ---- Libraries -------
set(libs)
file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../src/*/*.cpp)
foreach(source ${sources})
    get_filename_component(src ${source} NAME_WE)
    message(INFO " adding target ${src} from ${source}")
    add_library(${src} STATIC ${source})
    set_target_properties(${src} PROPERTIES CXX_STANDARD 17 )
    target_include_directories(${src} PRIVATE "../include")
    list(APPEND libs ${src})
endforeach()

# ---- Targets ---------
file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${sources})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE "../include")
target_link_libraries(${PROJECT_NAME} PRIVATE benchmark Boost::circular_buffer ${libs})

