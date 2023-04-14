cmake_minimum_required(VERSION 3.15...3.22)

project(fixed_size_string_buffer LANGUAGES CXX)

# --- Import tools ----
include(../cmake/common.cmake)

# ---- Create standalone executables ----
file(GLOB targets CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../standalone/source/*.cpp)

find_package(${PROJECT_NAME} REQUIRED)

foreach(tgt ${targets})
    get_filename_component(target ${tgt} NAME_WE)
    message(INFO " adding target ${target} from ${tgt}")
    add_executable(${target} ${tgt})
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17 )
    #set_target_warnings(${target})
    #target_include_directories(${target} PRIVATE ${CMAKE_INSTALL_PREFIX}/include)
    target_link_libraries(${target} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
endforeach()

