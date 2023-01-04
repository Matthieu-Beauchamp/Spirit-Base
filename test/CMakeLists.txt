enable_testing()

macro(spirit_base_add_test testName ...)
    spirit_add_test(spirit-base ${testName} ${...})
endmacro()

spirit_base_add_test(AnsiEscape-test testAnsiEscape.cpp)
spirit_base_add_test(Concepts-test testConcepts.cpp)
spirit_base_add_test(fileBuf-test testFileBuf.cpp)
spirit_base_add_test(ansiStream-test testAnsiStream.cpp)
spirit_base_add_test(Logger-test testLogger.cpp)

# adds spirit-base-test
spirit_test_all(spirit-base)
