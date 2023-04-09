#
# generals cmake utils
#

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
