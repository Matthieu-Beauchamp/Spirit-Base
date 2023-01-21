include(Macros)


############################################################
# spdlog
############################################################
function(build_spdlog spdlog_root target)
add_subdirectory(${spdlog_root})
    target_include_directories(${target} PUBLIC ${spdlog_root}/include)
    target_link_libraries(${target} PUBLIC spdlog)
endfunction()


############################################################
# Boost
############################################################
function(build_Boost boost_root target)
    # Currently public for testLogger using boost::source_location
    target_include_directories(spirit-base PRIVATE ${boost_root})

    set(B2_FLAGS --with-stacktrace)

    if (${BUILD_SHARED_LIBS})
        set(B2_FLAGS ${B2_FLAGS} link=shared)    
    else()
        set(B2_FLAGS ${B2_FLAGS} link=static)    
    endif()

    if(${MSVC})
        set(B2_FLAGS ${B2_FLAGS} --layout=tagged)    
    endif()

    # TODO: Use the same compiler ...

    macro(compileFlag flag)
        target_compile_definitions(${target} PRIVATE ${flag})
        set(B2_FLAGS ${B2_FLAGS} cxxflags=-D${flag})    
    endmacro()

    set(BOOST_LINK_DIR "${boost_root}/stage/lib")
    
    compileFlag(BOOST_STACKTRACE_LINK)
    if (${MSVC})
        compileFlag(BOOST_STACKTRACE_USE_WINDBG)
    else()
        compileFlag(BOOST_STACKTRACE_USE_BACKTRACE)
    endif()

    if (NOT EXISTS "${BOOST_LINK_DIR}")
        message(STATUS "Boost link directory not found at: ${BOOST_LINK_DIR}")
        if (EXISTS "${boost_root}")
            message(STATUS "Compiling Boost libraries")

            if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
                execute_process(COMMAND "bootstrap.bat" 
                    WORKING_DIRECTORY "${boost_root}")

                execute_process(COMMAND "./b2.exe" ${B2_FLAGS}
                    WORKING_DIRECTORY "${boost_root}")
            else ()

                execute_process(COMMAND "./bootstrap.sh" 
                    WORKING_DIRECTORY "${boost_root}")
            
                execute_process(COMMAND "./b2"  ${B2_FLAGS}
                    WORKING_DIRECTORY "${boost_root}")
            endif ()

            if (EXISTS "${BOOST_LINK_DIR}")
                message(STATUS "Boost built successfully")
            endif()

        else ()
            message(FATAL_ERROR "Invalid boost root directory: ${boost_root}")
        endif ()
    endif ()

    # Public for shared libraries..?
    target_link_directories(${target} PUBLIC "${BOOST_LINK_DIR}") # Insists on linking other targets with msvc
    # message(STATUS "${CMAKE_BUILD_TYPE} => ${LIB_NAME}")

    # TODO: b2 with msvc builds about a dozen of these, and then CMake 
    # cannot link to simply "boost_stacktrace_windbg".
    # Here we choose one, this is not portable. 
    # If we can detect if building for x86 or x64, we could at least adapt a bit more
    if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        if(${MSVC})
            target_link_libraries(${target} PUBLIC 
    	        $<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P}, 4>
                    # 32 bits
                    $<IF:$<CONFIG:Debug>, libboost_stacktrace_windbg-mt-gd-x32, 
                                          libboost_stacktrace_windbg-mt-x32
                    >,
                    # else 64 bits
                    $<IF:$<CONFIG:Debug>, libboost_stacktrace_windbg-mt-gd-x64, 
                                          libboost_stacktrace_windbg-mt-x64
                    >
                >    
    	    )
        else()  # ? what about mingw, etc..?
    	    
        endif()
    else()
        target_link_libraries(${target} PUBLIC boost_stacktrace_backtrace)
    endif()
endfunction()

