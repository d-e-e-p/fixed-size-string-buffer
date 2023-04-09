cmake_minimum_required(VERSION 3.15...3.22)

project(fixed_size_string_buffer LANGUAGES CXX)


# ---- Settings --------
set (ENABLE_CLANG_TIDY ON)
set (ENABLE_CPPCHECK ON)

# --- Import tools ----
include(../cmake/common.cmake)

# ---- Create standalone executables ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

foreach(src ${sources})
    get_filename_component(target ${src} NAME_WE)
    message(INFO " adding target ${target} from ${src}")
    add_executable(${target} ${src})
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17 )
    set_target_warnings(${target})
    target_include_directories(${target} PRIVATE "../include")

    # https://stackoverflow.com/questions/543203/cmake-runtime-output-directory-on-windows
    # hack to get around the "Debug" and "Release" directories cmake tries to add on Windows
    if (MSVC)
        #set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "$<1:${PROJECT_BINARY_DIR}/bin>")
    endif()
endforeach()


file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*boost*.cpp)

foreach(src ${sources})
    get_filename_component(target ${src} NAME_WE)
    message(INFO " adding boost to target ${target} from ${src}")

    find_package(Boost REQUIRED)
    target_link_libraries(${target} ${Boost_LIBRARIES})
    target_include_directories(${target} PRIVATE ${Boost_INCLUDE_DIRS})

endforeach()
enable_sanitizer("Address")





