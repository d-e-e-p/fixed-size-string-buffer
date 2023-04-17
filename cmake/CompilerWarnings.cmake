# from here:
#
# https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
# Courtesy of Jason Turner

function(enable_warnings)
    get_compiler_warnings(_flags)
    add_compile_options("${_flags}")
    message(STATUS "enable_warnings: adding compiler flags ${_flags}")
endfunction()


function(set_target_warnings target_name)
    get_compiler_warnings(_flags)
    if(BUILD_HEADERS_ONLY)
          target_compile_options(${target_name} INTERFACE ${_flags})
    else()
          target_compile_options(${target_name} PUBLIC ${_flags})
    endif()
  
    if(NOT TARGET ${target_name})
      message(AUTHOR_WARNING " ${target_name} is not a target, so no compiler warning were added.")
    endif()
endfunction()

function(get_compiler_warnings flags)

  set(MSVC_WARNINGS
      /W4     # Baseline reasonable warnings
      /w14242 # 'identifier': conversion from 'type1' to 'type1', possible loss
              # of data
      /w14254 # 'operator': conversion from 'type1:field_bits' to
              # 'type2:field_bits', possible loss of data
      /w14263 # 'function': member function does not override any base class
              # virtual member function
      /w14265 # 'classname': class has virtual functions, but destructor is not
              # virtual instances of this class may not be destructed correctly
      /w14287 # 'operator': unsigned/negative constant mismatch
      /we4289 # nonstandard extension used: 'variable': loop control variable
              # declared in the for-loop is used outside the for-loop scope
      /w14296 # 'operator': expression is always 'boolean_value'
      /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
      /w14545 # expression before comma evaluates to a function which is missing
              # an argument list
      /w14546 # function call before comma missing argument list
      /w14547 # 'operator': operator before comma has no effect; expected
              # operator with side-effect
      /w14549 # 'operator': operator before comma has no effect; did you intend
              # 'operator'?
      /w14555 # expression has no effect; expected expression with side- effect
      /w14619 # pragma warning: there is no warning number 'number'
      /w14640 # Enable warning on thread un-safe static member initialization
      /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may
              # cause unexpected runtime behavior.
      /w14905 # wide string literal cast to 'LPSTR'
      /w14906 # string literal cast to 'LPWSTR'
      /w14928 # illegal copy-initialization; more than one user-defined
              # conversion has been implicitly applied
      /permissive- # standards conformance mode for MSVC compiler.
      /wd4068 # /wd<n>  unknown pragma 'GCC' 
      /D _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
  )

  set(UNIX_WARNINGS
      -Wall
      -Wextra  # reasonable and standard
      -Wshadow # warn the user if a variable declaration shadows one from a
               # parent context
      -Wnon-virtual-dtor # warn the user if a class with virtual functions has a
                         # non-virtual destructor. This helps catch hard to
                         # track down memory errors
      -Wold-style-cast # warn for c-style casts
      -Wcast-align     # warn for potential performance problem casts
      -Wunused         # warn on anything being unused
      -Woverloaded-virtual # warn if you overload (not override) a virtual
                           # function
      -Wpedantic   # warn if non-standard C++ is used
      -Wconversion # warn on type conversions that may lose data
      -Wsign-conversion  # warn on sign conversions
      -Wnull-dereference # warn if a null dereference is detected
      -Wdouble-promotion # warn if float is implicit promoted to double
      -Wformat=2 # warn on security issues around functions that format output
                 # (ie printf)
      -Wno-narrowing
  )


  set(GCC_WARNINGS
      ${UNIX_WARNINGS}
      -Wmisleading-indentation # warn if indentation implies blocks where blocks
                               # do not exist
      -Wduplicated-cond # warn if if / else chain has duplicated conditions
      -Wduplicated-branches # warn if if / else branches have duplicated code
      -Wlogical-op   # warn about logical operations being used where bitwise were
                     # probably wanted
      -Wuseless-cast # warn if you perform a cast to the same type
  )

  set(CLANG_WARNINGS ${UNIX_WARNINGS})
  if(uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG")
      # warn if doxygen documentation tags are incorrect
      string(CONCAT CLANG_WARNINGS -Wdocumentation)
  endif()

  if (WARNINGS_AS_ERRORS)
    set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
    set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)
  endif()

  if(MSVC)
    set(PROJECT_WARNINGS ${MSVC_WARNINGS})
  elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(PROJECT_WARNINGS ${CLANG_WARNINGS})
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(PROJECT_WARNINGS ${GCC_WARNINGS})
  else()
    message(AUTHOR_WARNING "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
  endif()

  set(${flags} ${PROJECT_WARNINGS} PARENT_SCOPE)

endfunction()
