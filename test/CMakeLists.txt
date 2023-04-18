# cmake test
cmake_minimum_required(VERSION 3.15...3.22)
include(../cmake/common.cmake)

# --- Setup project ---
project(unit_tests LANGUAGES CXX)
common_compiler_settings()
enable_testing()


# ---Setup GoogleTest --

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(CMAKE_BUILD_TYPE "Debug")

#FetchContent_MakeAvailable(googletest-src)
CPMAddPackage("gh:google/googletest@1.13.0")

# ---Config Lib Src ----
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

# ---Config Test Src ---
file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
message(STATUS "sources = ${sources} dir=${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(${PROJECT_NAME} ${sources})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE "../include")
target_link_libraries(${PROJECT_NAME} gtest_main gmock_main ${libs}) 
add_test(NAME tests COMMAND ${PROJECT_NAME})

if(ENABLE_COVERAGE)
    enable_coverage()
endif()
