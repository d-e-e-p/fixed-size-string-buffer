#
# shared by all cmake runs
#

# --- Settings  -------

# at flag at compile time in Debug config
#add_compile_definitions("$<$<NOT:$<CONFIG:Debug>>:NDEBUG>")
set(PROJECT fixed-size-string-buffer)
set(VERSION 1.4.0)
set(LANGUAGE CXX)
set(DESCRIPTION "A C++ pre-allocated ring buffer for std::string messages")
set(HOMEPAGE_URL "https://github.com/d-e-e-p/fixed-size-string-buffer/wiki")

# --- Import tools ----
# prefer actual filenames over CMAKE_MODULE_PATH + magic include
set(dir ${CMAKE_CURRENT_LIST_DIR})

include(${dir}/Utils.cmake)
include(${dir}/CompilerWarnings.cmake)
include(${dir}/StaticAnalyzers.cmake)

download_CPM_cmake()
include(${dir}/CPM.cmake)

include(${dir}/Sanitizers.cmake)
include(${dir}/Coverage.cmake)

# ---- settings --------
init_output_directories()             # make all exe live under bin/
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json 
set(CPM_SOURCE_CACHE "${dir}/../.cache/CPM") # store all downloaded packages

# make windows unicode work
if(MSVC)
    add_compile_options(/std:c++latest /utf-8)
    message(STATUS "adding compile option /utf-8")
endif()

# --- Import tools ----
# CPMAddPackage("gh:TheLartians/Ccache.cmake@1.2.3")


