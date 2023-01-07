
macro(spirit_base_benchmark targetName ...)
    spirit_benchmark(spirit-base ${targetName} ${...})
endmacro()

# TODO: These benchmarks should not use stdout (caps performance)
spirit_base_benchmark(streamableMessage-benchmark streamableMessages.cpp)
spirit_base_benchmark(ansiParsing-benchmark ansiEscapeParsing.cpp)

spirit_analyse_benchmarks(spirit-base ${CMAKE_CURRENT_SOURCE_DIR}/out)