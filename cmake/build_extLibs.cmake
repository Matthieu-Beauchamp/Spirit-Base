include(Macros)

############################################################
# spdlog
############################################################
function(build_spdlog spdlog_root libsVar includeVar)
    add_subdirectory(${spdlog_root})
    set(${includeVar} ${spdlog_root}/include PARENT_SCOPE)

    set(${libsVar} spdlog PARENT_SCOPE)
    return(PROPAGATE ${libsVar} ${includeVar})
endfunction()


############################################################
# Boost
############################################################
function(build_Boost boost_root libsVar includeDirVar linkDirVar)
    set(boost_link_dir "${boost_root}/stage/lib")

    # Too bad for windows
    add_compile_definitions(BOOST_STACKTRACE_LINK)
    if (${MSVC})
        add_compile_definitions(BOOST_STACKTRACE_USE_WINDBG)
        set(LIB_NAME boost_stacktrace_windbg)
    else()
        add_compile_definitions(BOOST_STACKTRACE_USE_BACKTRACE)
        set(LIB_NAME boost_stacktrace_backtrace)
    endif()

    if (NOT EXISTS "${boost_link_dir}")
        message("Boost link directory not found at: ${boost_link_dir}")
        if (EXISTS "${boost_root}")
            message("Compiling Boost libraries")

            if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
                execute_process(COMMAND "bootstrap.bat" 
                    WORKING_DIRECTORY "${boost_root}")

                execute_process(COMMAND "./b2.exe"
                    WORKING_DIRECTORY "${boost_root}")
            else ()
                execute_process(COMMAND "./bootstrap.sh" 
                    WORKING_DIRECTORY "${boost_root}")
                
                execute_process(COMMAND "./b2"
                    WORKING_DIRECTORY "${boost_root}")
            endif ()

            if (EXISTS "${boost_link_dir}")
                message("Boost built successfully")
            endif()

        else ()
            message(FATAL_ERROR "Invalid boost root directory: ${boost_root}")
        endif ()
    endif ()

    set(${libVar} ${LIB_NAME} PARENT_SCOPE)
    set(${includeDirVar} ${boost_root} PARENT_SCOPE)
    set(${linkDirVar} ${boost_link_dir} PARENT_SCOPE) # I don't like this too much (see target_link...)

    return(PROPAGATE 
        ${libVar}
        ${includeDirVar}
        ${linkDirVar}
    )
endfunction()

