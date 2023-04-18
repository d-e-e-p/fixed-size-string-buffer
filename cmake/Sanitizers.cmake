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
      /fsanitize=address                     #
      #/fsanitize-address-use-after-return   # 
      /MTd  # link into exe
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
        set(CFLAGS ${MSVC_SANITIZERS})
        set(LFLAGS /INFERASANLIBS /ignore:4075)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(CFLAGS ${CLANG_SANITIZERS})
        set(LFLAGS ${CLANG_SANITIZERS})
    else()
        set(CFLAGS ${GCC_SANITIZERS})
        set(LFLAGS ${GCC_SANITIZERS})
    endif()

    message(STATUS "enable_sanitizers: adding flags ${CFLAGS}")
    add_compile_options("$<$<CONFIG:Debug>:${CFLAGS}>")
    add_link_options("$<$<CONFIG:Debug>:${LFLAGS}>")

endfunction()

