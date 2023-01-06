function(celero_include_internal targetName)
    target_include_directories(${targetName} PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../benchmarks/Celero/include/)
endfunction()

macro(spirit_benchmark spiritLib targetName ...)
    add_executable(${targetName} ${...})
    celero_include_internal(${targetName})
    target_link_libraries(${targetName} celero)

    set(benchSrcsVar ${spiritLib}-benchSrcs)
    set(${benchSrcsVar} ${${benchSrcsVar}} ${targetName})
endmacro()


function(spirit_plot_internal outputDir csv html)
    set(plotScript ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/plotBenchmark.py)
    find_package(Python3)
    if (NOT Python3_FOUND)
        message(FATAL_ERROR "Could not find a python3 interpreter")
    endif()

    add_custom_command(
        OUTPUT ${html}
        COMMAND ${Python3_EXECUTABLE} ${plotScript} ${csv} ${html}
        WORKING_DIRECTORY ${outputDir}
        DEPENDS ${csv}
    )

endfunction()


# outputDir should be an absolute path
macro(spirit_analyse_benchmarks spiritLib outputDir)
    set(benchSrcsVar ${spiritLib}-benchSrcs)
    # call script on all

    if (NOT IS_DIRECTORY "${outputDir}")
        file(MAKE_DIRECTORY "${outputDir}")
    endif()
    
    foreach(benchmark ${${benchSrcsVar}})
        set(csv ${benchmark}.csv)

        add_custom_command(
            OUTPUT ${csv}
            COMMAND ${benchmark} -t ${csv}
            WORKING_DIRECTORY ${outputDir}
        )

        set(html ${benchmark}.html)
        set(htmls ${htmls} ${html})
        spirit_plot_internal(${outputDir} ${csv} ${html})

    endforeach()
          
    add_custom_target(
        ${spiritLib}-benchmarks
        DEPENDS ${htmls}
    )
endmacro()