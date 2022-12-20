# parent scope assignation
macro(assign var value)
    set(${var} ${value} PARENT_SCOPE)
endmacro()

function(spirit_is_debug var)
    if (${CMAKE_BUILD_TYPE} MATCHES Debug)
        assign(${var} TRUE)
    else ()
        assign(${var} FALSE)
    endif ()
endfunction()


function (appendSource var ...)
    set(${var} ${...})
    return(PROPAGATE ${var})
endfunction()


# Taken from SFML ##########################################
macro(spirit_define_option var default type docstring)
    if (NOT DEFINED ${var})
        set(${var} ${default})
    endif ()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()
############################################################


find_package(Git)

function(spirit_update_submodules dir)
if (${Git_FOUND})
    execute_process(
                COMMAND "${GIT_EXECUTABLE}" submodule update --init --recursive . # git pull --recurse-submodules 
                WORKING_DIRECTORY "${dir}"
                COMMAND_ECHO STDOUT
                OUTPUT_STRIP_TRAILING_WHITESPACE
                )

                message("update submodules output from ${dir}:\n${out}")
else()
    message("Could not find git, Modules may not be up to date")
endif()
endfunction()