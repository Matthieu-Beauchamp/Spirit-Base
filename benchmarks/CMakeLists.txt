
macro(spirit_base_benchmark targetName ...)
    spirit_benchmark(spirit-base ${targetName} ${...})
endmacro()

spirit_base_benchmark(streamableMessage streamableMessages.cpp)
spirit_analyse_benchmarks(spirit-base ${CMAKE_CURRENT_SOURCE_DIR}/out)