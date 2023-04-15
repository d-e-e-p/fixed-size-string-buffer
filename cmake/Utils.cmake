#
# generals cmake utils
#

# see https://markdewing.github.io/blog/posts/notes-on-cmake/
function(find_all_include_directories root_dir include_dirs)
  set(dirlist "")
  file(GLOB_RECURSE new_include_dirs LIST_DIRECTORIES true "${root_dir}/*")
  foreach(dir ${new_include_dirs})
    if(IS_DIRECTORY ${dir})
      if(${dir} MATCHES ".*/include$")
          #message(INFO " match dir = ${dir}")
        list(APPEND dirlist ${dir})
      endif()
    endif()
  endforeach()
  SET(${include_dirs} ${dirlist} PARENT_SCOPE)
  #message(INFO " find_all_include_directories:start ${root_dir} ${${include_dirs}}")
endfunction()

function(dump_all_vars)

    include(CMakePrintHelpers)
    get_cmake_property(_varNames VARIABLES)
    list (REMOVE_DUPLICATES _varNames)
    list (SORT _varNames)
    foreach (_varName ${_varNames})
        cmake_print_variables(${_varName})
    endforeach()

endfunction()

function(check_for_in_source_build)
    if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
        message(FATAL_ERROR
        "In-source builds not allowed. PROJECT_SOURCE_DIR=PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}"
        )
    endif()
endfunction()

function(download_CPM_cmake)
    set(DOWNLOAD_LOCATION ../cmake/CPM.cmake)
    get_filename_component(ABSOLUTE_PATH ${DOWNLOAD_LOCATION} ABSOLUTE)
    if(NOT (EXISTS ${ABSOLUTE_PATH}))
        message(INFO " downloading CPM.cmake to ${ABSOLUTE_PATH}")
        file(DOWNLOAD
            https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/cpm.cmake
            ${ABSOLUTE_PATH})
    endif()
endfunction()

function(download_Sanitizers_cmake)
    set(DOWNLOAD_LOCATION ../cmake/Sanitizers.cmake)
    get_filename_component(ABSOLUTE_PATH ${DOWNLOAD_LOCATION} ABSOLUTE)
    if(NOT (EXISTS ${ABSOLUTE_PATH}))
        message(INFO " downloading Sanitizers.cmake to ${ABSOLUTE_PATH}")
        file(DOWNLOAD
            https://raw.githubusercontent.com/StableCoder/cmake-scripts/main/sanitizers.cmake
            ${ABSOLUTE_PATH})
    endif()
endfunction()

# from https://stackoverflow.com/questions/31817291/cmake-generator-expression-for-runtime-output-directory
# initialize the variables defining output directories
#
# Sets the following variables:
#
# - :cmake:data:`CMAKE_ARCHIVE_OUTPUT_DIRECTORY`
# - :cmake:data:`CMAKE_LIBRARY_OUTPUT_DIRECTORY`
# - :cmake:data:`CMAKE_RUNTIME_OUTPUT_DIRECTORY`
#
# plus the per-config variants, ``*_$<CONFIG>``
#
# @public
#
macro(init_output_directories)

  # Directory for output files
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib 
    CACHE PATH "Output directory for static libraries.")

  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CACHE PATH "Output directory for shared libraries.")

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CACHE PATH "Output directory for executables and DLL's.")

  foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/bin" CACHE PATH "" FORCE)
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/lib" CACHE PATH "" FORCE)
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/lib" CACHE PATH "" FORCE)
  endforeach()

endmacro()

#execute_process(COMMAND ${GIT_EXECUTABLE} ls-remote --tags --sort=v:refname https://github.com/TheLartians/ModernCppStarter | tail -1 | sed 's/.*refs\/tags\/v//'

function(get_latest_git_tag rep RES)
    find_package(Git QUIET)

    set(pattern "s;refs/tags/v")
    set(CMD "${GIT_EXECUTABLE} ls-remote --refs --heads --tags --sort=v:refname ${rep} | tail -1 | sed 's:.*refs/tags/::' " )
    # set(CMD "${GIT_EXECUTABLE} ls-remote  --heads --tags --sort=v:refname ${rep} | tail -1 | sed 's/\\t.*//' ")
    execute_process(
        COMMAND bash "-c" ${CMD}
        OUTPUT_VARIABLE OUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message("latest tag: ${OUT}  from ${rep}")
    set(${RES} ${OUT} PARENT_SCOPE)

endfunction()


