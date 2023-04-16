#
# cmake add_sanitizers
#

include(CheckCXXSourceCompiles)


function(enable_sanitizers)
    string(TOLOWER "${CMAKE_BUILD_TYPE}" lowercase_CMAKE_BUILD_TYPE)
    if(NOT lowercase_CMAKE_BUILD_TYPE STREQUAL "debug" )
        message(STATUS "skipping sanitizers for build type ${CMAKE_BUILD_TYPE}")
        return()
    endif()

    set(MSVC_SANITIZERS
      /fsanitize=address                    #
      /fsanitize-address-use-after-return   # 
     )
    
     set(GCC_SANITIZERS
         -fsanitize=address 
         -fsanitize-address-use-after-scope
         -fsanitize=undefined  
     )

     set(CLANG_SANITIZERS
         ${GCC_SANITIZERS}
         -fsanitize=integer 
         -fsanitize=nullability
     )

    if (MSVC)
        set(FLAGS ${MSVC_SANITIZERS})
    elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(FLAGS ${CLANG_SANITIZERS})
    else()
        set(FLAGS ${GCC_SANITIZERS})
    endif()

    message(STATUS "enable_sanitizers: adding flags ${FLAGS}")
    add_compile_options("$<$<CONFIG:Debug>:${FLAGS}>")
    add_link_options("$<$<CONFIG:Debug>:${FLAGS}>")

endfunction()

