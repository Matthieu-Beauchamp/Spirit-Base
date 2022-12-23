include(Macros)


############################################################
# spdlog
############################################################
function(build_spdlog spdlog_root target)
add_subdirectory(${spdlog_root})

    # TODO: include directory should be made PRIVATE (all in .cpp files)
    target_include_directories(${target} PUBLIC ${spdlog_root}/include)
    target_link_libraries(${target} spdlog)
endfunction()


############################################################
# Boost
############################################################
function(build_Boost boost_root target)
    # Currently public for testLogger using boost::source_location
    target_include_directories(spirit-base PUBLIC ${boost_root})

    set(B2_FLAGS "")
    macro(compileFlag flag)
        target_compile_definitions(${target} PRIVATE ${flag})
        set(B2_FLAGS "${B2_FLAGS} cxxflags=-D${flag}")    
    endmacro()

    set(BOOST_LINK_DIR "${boost_root}/stage/lib")
    
    compileFlag(BOOST_STACKTRACE_LINK)
    if (${MSVC})
        compileFlag(BOOST_STACKTRACE_USE_WINDBG)
        set(LIB_NAME boost_stacktrace_windbg)
    else()
        compileFlag(BOOST_STACKTRACE_USE_BACKTRACE)
        set(LIB_NAME boost_stacktrace_backtrace)
    endif()

    if (NOT EXISTS "${BOOST_LINK_DIR}")
        message("Boost link directory not found at: ${BOOST_LINK_DIR}")
        if (EXISTS "${boost_root}")
            message("Compiling Boost libraries")

            if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
                execute_process(COMMAND "bootstrap.bat" 
                    WORKING_DIRECTORY "${boost_root}")

                execute_process(COMMAND "./b2.exe ${B2_FLAGS}"
                    WORKING_DIRECTORY "${boost_root}")
            else ()

                execute_process(COMMAND "./bootstrap.sh" 
                    WORKING_DIRECTORY "${boost_root}")
            
                execute_process(COMMAND "./b2 ${B2_FLAGS}"
                    WORKING_DIRECTORY "${boost_root}"
                    COMMAND_ECHO STDOUT
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
            endif ()

            if (EXISTS "${BOOST_LINK_DIR}")
                message("Boost built successfully")
            endif()

        else ()
            message(FATAL_ERROR "Invalid boost root directory: ${boost_root}")
        endif ()
    endif ()

    # Public for shared libraries..?
    target_link_directories(${target} PUBLIC "${BOOST_LINK_DIR}")
    target_link_libraries(${target} ${LIB_NAME})
endfunction()

