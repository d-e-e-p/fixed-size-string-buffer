# validate cmake
cmake_minimum_required(VERSION 3.15...3.22)

project(fixed-size-string-buffer LANGUAGES CXX)

# --- Import tools ----
include(../cmake/common.cmake)

# ---- Create standalone executables ----
file(GLOB targets CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

# find_package doesn't automatically alias, so we need to do:
#    add_library(${PROJECT_NAME} ALIAS ${PROJECT_NAME}::${PROJECT_NAME})
# this 'feature' is exploited to load the lib both ways and test it out

# ---- test local install find_package version
find_package(${PROJECT_NAME} REQUIRED)
foreach(tgt ${targets})
    get_filename_component(target ${tgt} NAME_WE)
    set(target ${target}_fp)
    message(INFO " find_package: adding target ${target} from ${tgt}")
    add_executable(${target} ${tgt})
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17 )
    target_link_libraries(${target} PRIVATE  ${PROJECT_NAME})
endforeach()


# ---- run against latest version from git
set(rep https://github.com/d-e-e-p/fixed-size-string-buffer)
get_latest_git_tag(${rep} tag)
CPMAddPackage(NAME ${PROJECT_NAME} GIT_REPOSITORY ${rep} GIT_TAG ${tag})

foreach(tgt ${targets})
    get_filename_component(target ${tgt}_cpm NAME_WE)
    set(target ${target}_cpm)
    message(INFO " CPMAddPackage: adding target ${target} from ${tgt}")
    add_executable(${target} ${tgt})
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17 )
    target_link_libraries(${target} PRIVATE  ${PROJECT_NAME}::${PROJECT_NAME})
endforeach()

