cmake_minimum_required(VERSION 3.15...3.22)

project(fixed_size_string_buffer LANGUAGES CXX)


# ---- Settings --------
set (ENABLE_CLANG_TIDY ON)
set (ENABLE_CPPCHECK ON)

# --- Import tools ----
include(../cmake/common.cmake)


# example of adding library
#find_package(Boost REQUIRED)
#target_link_libraries(${target_name} ${Boost_LIBRARIES})
#target_include_directories(${target_name} PRIVATE ${Boost_INCLUDE_DIRS})
# ---- Create standalone executables ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

foreach(src ${sources})
    get_filename_component(target_name ${src} NAME_WE)
    message(INFO " adding target ${target_name} from ${src}")
    add_executable(${target_name} ${src})
    set_target_properties(${target_name} PROPERTIES CXX_STANDARD 17 )
    set_target_warnings(${target_name})
    target_include_directories(${target_name} PRIVATE "../include")
endforeach()



enable_sanitizer("Address")




