if(ENABLE_CLANG_TIDY)
  find_program(CLANGTIDY clang-tidy)
  if(CLANGTIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY} -extra-arg=-Wno-unknown-warning-option -extra-arg=-std=c++17 )
    message("Clang-Tidy finished setting up.")
  else()
      message(STATUS " Clang-Tidy not found so check skipped")
  endif()
endif()

if(ENABLE_CPPCHECK)
  find_program(CPPCHECK cppcheck)
  if(CPPCHECK)
    set(CMAKE_CXX_CPPCHECK ${CPPCHECK} --suppress=missingInclude --enable=all
                           --inline-suppr --inconclusive -i ${CMAKE_SOURCE_DIR}/imgui/lib)
    message("Cppcheck finished setting up.")
  else()
      message(STATUS " Cppcheck not found so check skipped")
  endif()
endif()
