add_executable(AnsiCodesExample
        "AnsiCodes.cpp"
        )

target_link_libraries(AnsiCodesExample spirit-base)

add_executable(AnsiColorsExample
        "AnsiColors.cpp"
        )

target_link_libraries(AnsiColorsExample spirit-base)
