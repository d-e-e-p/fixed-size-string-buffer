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

function(download_CPM_dot_cmake)
    set(CPM_DOWNLOAD_LOCATION ../cmake/CPM.cmake)
    get_filename_component(ABSOLUTE_PATH ${CPM_DOWNLOAD_LOCATION} ABSOLUTE)
    if(NOT (EXISTS ${ABSOLUTE_PATH}))
        message(INFO " downloading CPM.cmake to ${ABSOLUTE_PATH}")
        file(DOWNLOAD
            https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/cpm.cmake
            ${ABSOLUTE_PATH})
    endif()
endfunction()
