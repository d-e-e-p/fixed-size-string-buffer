# enable analysers cmake
# eg enable_analyzers(clang-tidy cppcheck cpplint include-what-you-use)

function(enable_analyzers)

    if(${ARGC} EQUAL 0)
        # default
        set(ENABLE_ANALYSER clang-tidy cppcheck cpplint include-what-you-use)
    else()
        set(ENABLE_ANALYSER ${ARGV})
    endif()

    message(DEBUG " enable_analyzers: enabling ${ENABLE_ANALYSER}")

    set(flags_clang-tidy -extra-arg=-Wno-unknown-warning-option -extra-arg=-std=c++17)
    set(flags_cppcheck --suppress=missingInclude --enable=all --inline-suppr --inconclusive --std=c++17)
    set(flags_cpplint)
    set(flags_include-what-you-use -Xiwyu --cxx17ns -Xiwyu --no_fwd_decls -Xiwyu --max_line_length=132 -Xiwyu
        --verbose=1)

    # for free!
    set(CMAKE_LINK_WHAT_YOU_USE ON PARENT_SCOPE)

    foreach(_analyser ${ENABLE_ANALYSER})
      #find_program(EXE ${_analyser})
      unset(tool)
      find_program(tool NAMES ${_analyser} NO_CACHE)
      if(tool)
        string(REPLACE "-" "_" _name ${_analyser})
        string(TOUPPER ${_name} _NAME)
        set(CMAKE_CXX_${_NAME} ${tool} ${flags_${_analyser}} PARENT_SCOPE)
        message(STATUS " enable_analyzers: ${_analyser} ${flags_${_analyser}}")
      else()
        message(STATUS " enable_analyzers: ${_analyser} not found so check skipped")
      endif()
    endforeach()


endfunction()
