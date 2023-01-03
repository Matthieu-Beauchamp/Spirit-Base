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
                )
else()
    message("Could not find git, Modules may not be up to date")
endif()
endfunction()