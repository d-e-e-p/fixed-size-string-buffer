# common compiler settings

function(common_compiler_settings)

    init_output_directories()                          # make all exe live under bin/
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON PARENT_SCOPE) # Generate compile_commands.json 

    # make windows unicode work
    if(MSVC)
        set(OPT /utf-8)
        add_compile_options(${OPT})
        message(STATUS "adding compile option ${OPT}")
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
