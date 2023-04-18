#
# general cmake utils
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

function(print_all_vars)
    include(CMakePrintHelpers)
    get_cmake_property(_varNames VARIABLES)
    list (REMOVE_DUPLICATES _varNames)
    list (SORT _varNames)
    foreach (_varName ${_varNames})
        cmake_print_variables(${_varName})
    endforeach()

endfunction()

# dump all known properties of all targets
function(print_all_properties)
    include(CMakePrintHelpers)
    foreach(_target DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
        get_property(_properties TARGET ${_target} PROPERTY VARIABLES)
        foreach(_property _properties)
            cmake_print_properties(TARGETS ${_target} PROPERTIES ${_property})
        endforeach()
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


#
# from https://stackoverflow.com/questions/32183975/how-to-print-all-the-properties-of-a-target-in-cmake
#

function(print_target_properties tgt)
    if(NOT TARGET ${tgt})
      message("There is no target named '${tgt}'")
      return()
    endif()

    # Get all propreties that cmake supports
    execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)

    # Convert command output into a CMake list
    STRING(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    STRING(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    # Fix https://stackoverflow.com/questions/32197663/how-can-i-remove-the-the-location-property-may-not-be-read-from-target-error-i
    list(FILTER CMAKE_PROPERTY_LIST EXCLUDE REGEX "^LOCATION$|^LOCATION_|_LOCATION$")
    # For some reason, "TYPE" shows up twice - others might too?
    list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)

    # build whitelist by filtering down from CMAKE_PROPERTY_LIST in case cmake is
    # a different version, and one of our hardcoded whitelisted properties
    # doesn't exist!
    unset(CMAKE_WHITELISTED_PROPERTY_LIST)
    foreach(prop ${CMAKE_PROPERTY_LIST})
        if(prop MATCHES "^(INTERFACE|[_a-z]|IMPORTED_LIBNAME_|MAP_IMPORTED_CONFIG_)|^(COMPATIBLE_INTERFACE_(BOOL|NUMBER_MAX|NUMBER_MIN|STRING)|EXPORT_NAME|IMPORTED(_GLOBAL|_CONFIGURATIONS|_LIBNAME)?|NAME|TYPE|NO_SYSTEM_FROM_IMPORTED)$")
            list(APPEND CMAKE_WHITELISTED_PROPERTY_LIST ${prop})
        endif()
    endforeach(prop)

    get_target_property(target_type ${tgt} TYPE)
    if(target_type STREQUAL "INTERFACE_LIBRARY")
        set(PROP_LIST ${CMAKE_WHITELISTED_PROPERTY_LIST})
    else()
        set(PROP_LIST ${CMAKE_PROPERTY_LIST})
    endif()

    foreach (prop ${PROP_LIST})
        string(REPLACE "<CONFIG>" "${CMAKE_BUILD_TYPE}" prop ${prop})
        # message ("Checking ${prop}")
        get_property(propval TARGET ${tgt} PROPERTY ${prop} SET)
        if (propval)
            get_target_property(propval ${tgt} ${prop})
            message ("${tgt} ${prop} = ${propval}")
        endif()
    endforeach(prop)
endfunction(print_target_properties)
