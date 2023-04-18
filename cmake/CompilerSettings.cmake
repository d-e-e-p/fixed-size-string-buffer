# common compiler settings

function(compiler_settings)

    init_output_directories()   # make all exe live under bin/
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON PARENT_SCOPE) # Generate compile_commands.json 

    # make windows unicode work
    if(MSVC)
        set(OPT /utf-8)
        add_compile_options(${OPT})
        message(STATUS "adding compile option ${OPT}")
    endif()

endfunction()
