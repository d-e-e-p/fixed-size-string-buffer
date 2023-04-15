# validate cmake
cmake_minimum_required(VERSION 3.15...3.22)

project(fixed-size-string-buffer LANGUAGES CXX)

# --- Import tools ----
include(../cmake/common.cmake)

# ---- Create standalone executables ----
file(GLOB targets CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

# find_package doesn't automatically alias, so you can do
#    add_library(${PROJECT_NAME} ALIAS ${PROJECT_NAME}::${PROJECT_NAME})
# this 'feature' is exploited to load the lib both ways and test it out

find_package(${PROJECT_NAME} REQUIRED)
foreach(tgt ${targets})
    get_filename_component(target ${tgt} NAME_WE)
    set(target ${target}_fp)
    message(INFO " adding target ${target} from ${tgt}")
    add_executable(${target} ${tgt})
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17 )
    target_link_libraries(${target} PRIVATE  ${PROJECT_NAME})
endforeach()

CPMAddPackage("gh:d-e-e-p/${PROJECT_NAME}@1.1.0")
foreach(tgt ${targets})
    get_filename_component(target ${tgt}_cpm NAME_WE)
    set(target ${target}_cpm)
    message(INFO " adding target ${target} from ${tgt}")
    add_executable(${target} ${tgt})
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17 )
    target_link_libraries(${target} PRIVATE  ${PROJECT_NAME}::${PROJECT_NAME})
endforeach()

