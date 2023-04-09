#
# shared by all cmake runs
#


# --- Import tools ----
# prefer actual filenames over CMAKE_MODULE_PATH
include(../cmake/Utils.cmake)
include(../cmake/CompilerWarnings.cmake)
include(../cmake/StaticAnalyzers.cmake)
include(../cmake/Coverage.cmake)

download_CPM_cmake()
include(../cmake/CPM.cmake)

include(../cmake/Sanitizers.cmake)

# --- Import tools ----
# CPMAddPackage("gh:TheLartians/Ccache.cmake@1.2.3")


# ---- Dependencies ----
include_directories(. ../include)


# ---- settings --------
init_output_directories()
# Generate compile_commands.json for clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CPM_SOURCE_CACHE "../.cache/CPM")

# make windows unicode work
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /utf-8")
endif()
