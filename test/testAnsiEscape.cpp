#include "SPIRIT/Logging/AnsiEscape.hpp"
#include "catch2/catch_test_macros.hpp"

TEST_CASE("Ansi Escapes")
{
    SECTION("AnsiEscape Detection")
    {
        REQUIRE(sp::traits::isAnsiEscape<sp::AnsiEscape>::value);
        REQUIRE(sp::traits::isAnsiEscape<sp::FgColor>::value);
        REQUIRE(sp::traits::isAnsiEscape<sp::BgColor>::value);
        REQUIRE(sp::traits::isAnsiEscape<sp::Style>::value);
        REQUIRE(sp::traits::isAnsiEscape<sp::RgbFgColor>::value);
        REQUIRE(sp::traits::isAnsiEscape<sp::RgbBgColor>::value);
        REQUIRE(sp::traits::isAnsiEscape<sp::FgGradient>::value);
        REQUIRE(sp::traits::isAnsiEscape<sp::BgGradient>::value);
        REQUIRE(sp::traits::isAnsiEscape<sp::Bell>::value);
        REQUIRE(sp::traits::isAnsiEscape<sp::MoveCursorTo>::value);
        REQUIRE(sp::traits::isAnsiEscape<sp::CursorUp>::value);
        REQUIRE(sp::traits::isAnsiEscape<sp::EraseLine>::value);

        REQUIRE(sp::traits::isAnsiEscape<const sp::AnsiEscape &>::value);
        REQUIRE(sp::traits::isAnsiEscape<const sp::FgColor &>::value);
        REQUIRE(sp::traits::isAnsiEscape<const sp::BgColor &>::value);
        REQUIRE(sp::traits::isAnsiEscape<const sp::Style &>::value);

        REQUIRE(sp::traits::isAnsiEscape<volatile sp::AnsiEscape &&>::value);
        REQUIRE(sp::traits::isAnsiEscape<volatile sp::FgColor &&>::value);
        REQUIRE(sp::traits::isAnsiEscape<volatile sp::BgColor &&>::value);
        REQUIRE(sp::traits::isAnsiEscape<volatile sp::Style &&>::value);

        REQUIRE_FALSE(sp::traits::isAnsiEscape<int>::value);
        REQUIRE_FALSE(sp::traits::isAnsiEscape<const char *>::value);
        REQUIRE_FALSE(sp::traits::isAnsiEscape<unsigned char &>::value);
        REQUIRE_FALSE(sp::traits::isAnsiEscape<float>::value);
        REQUIRE_FALSE(sp::traits::isAnsiEscape<volatile std::string>::value);
        REQUIRE_FALSE(sp::traits::isAnsiEscape<void>::value);
        REQUIRE_FALSE(sp::traits::isAnsiEscape<void *>::value);
    }

    SECTION("TextStyle detection")
    {
        REQUIRE(sp::traits::isTextStyle<sp::FgColor>::value);
        REQUIRE(sp::traits::isTextStyle<sp::BgColor>::value);
        REQUIRE(sp::traits::isTextStyle<sp::Style>::value);
        REQUIRE(sp::traits::isTextStyle<sp::RgbFgColor>::value);
        REQUIRE(sp::traits::isTextStyle<sp::RgbBgColor>::value);
        REQUIRE(sp::traits::isTextStyle<sp::FgGradient>::value);
        REQUIRE(sp::traits::isTextStyle<sp::BgGradient>::value);

        REQUIRE_FALSE(sp::traits::isTextStyle<sp::AnsiEscape>::value);
        REQUIRE_FALSE(sp::traits::isTextStyle<sp::Bell>::value);
        REQUIRE_FALSE(sp::traits::isTextStyle<sp::MoveCursorTo>::value);
        REQUIRE_FALSE(sp::traits::isTextStyle<sp::CursorUp>::value);
        REQUIRE_FALSE(sp::traits::isTextStyle<sp::EraseLine>::value);
    }

    SECTION("TerminalControl detection")
    {
        REQUIRE_FALSE(sp::traits::isTerminalControl<sp::AnsiEscape>::value);
        REQUIRE_FALSE(sp::traits::isTerminalControl<sp::FgColor>::value);
        REQUIRE_FALSE(sp::traits::isTerminalControl<sp::BgColor>::value);
        REQUIRE_FALSE(sp::traits::isTerminalControl<sp::Style>::value);
        REQUIRE_FALSE(sp::traits::isTerminalControl<sp::RgbFgColor>::value);
        REQUIRE_FALSE(sp::traits::isTerminalControl<sp::RgbBgColor>::value);
        REQUIRE_FALSE(sp::traits::isTerminalControl<sp::FgGradient>::value);
        REQUIRE_FALSE(sp::traits::isTerminalControl<sp::BgGradient>::value);

        REQUIRE(sp::traits::isTerminalControl<sp::Bell>::value);
        REQUIRE(sp::traits::isTerminalControl<sp::MoveCursorTo>::value);
        REQUIRE(sp::traits::isTerminalControl<sp::CursorUp>::value);
        REQUIRE(sp::traits::isTerminalControl<sp::EraseLine>::value);
    }

    SECTION("Combined escapes")
    {
        REQUIRE(sp::traits::isTextStyle<sp::Escapes<sp::RgbFgColor>>::value);

        // REQUIRE(sp::traits::isTextStyle<
        //         sp::Escapes<sp::RgbFgColor, sp::Style, sp::BgGradient>>::value);

        // REQUIRE(sp::traits::isTerminalControl<
        //         sp::Escapes<sp::EraseLine, sp::CarriageRet>>::value);

        // REQUIRE(sp::traits::isTerminalControl<
        //         sp::Escapes<sp::EraseLine, sp::MoveCursorTo>>::value);

        // REQUIRE(
        //     sp::traits::isAnsiEscape<sp::Escapes<sp::FgColor, sp::EraseScreen>>::value
        // );

        // should not compile
        // REQUIRE_FALSE(sp::traits::isAnsiEscape<sp::Escapes<int>>::value);
    }
}