#
# shared by all cmake runs
#


# --- Import tools ----
include(../cmake/tools.cmake)
include(../cmake/utils.cmake)
include(../cmake/CompilerWarnings.cmake)
include(../cmake/StaticAnalyzers.cmake)
include(../cmake/Coverage.cmake)

download_CPM_dot_cmake()
include(../cmake/CPM.cmake)

# --- Import tools ----
# CPMAddPackage("gh:TheLartians/Ccache.cmake@1.2.3")


# ---- Dependencies ----
include_directories(. ../include)


# ---- settings --------

# Generate compile_commands.json for clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CPM_SOURCE_CACHE "../.cache/CPM")
