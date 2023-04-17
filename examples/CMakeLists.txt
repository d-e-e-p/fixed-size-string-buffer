# debug/release cmake
cmake_minimum_required(VERSION 3.15...3.22)

project(fixed-size-string-buffer LANGUAGES CXX)

# ---- Settings --------
set (ENABLE_CLANG_TIDY ON)
set (ENABLE_CPPCHECK ON)

# --- Import tools ----
include(../cmake/common.cmake)
enable_sanitizers()
enable_warnings()

# ---- Create standalone executables ----


#file(GLOB headers CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../include/*/*.h)
file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../src/*/*.cpp)
file(GLOB targets CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(libs)
foreach(source ${sources})
    get_filename_component(src ${source} NAME_WE)
    message(INFO " adding lib ${src} from ${source}")
    add_library(${src} STATIC ${source})
    set_target_properties(${src} PROPERTIES CXX_STANDARD 17 )
    target_include_directories(${src} PRIVATE "../include")
    list(APPEND libs ${src})
endforeach()

foreach(target ${targets})
    get_filename_component(tgt ${target} NAME_WE)
    message(INFO " adding exe ${tgt} from ${target}")
    add_executable(${tgt} ${target})
    set_target_properties(${tgt} PROPERTIES CXX_STANDARD 17 )
    target_include_directories(${tgt} PRIVATE "../include")
    target_link_libraries(${tgt} ${libs})
endforeach()






