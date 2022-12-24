#include "Catch2/catch_test_macros.hpp"
#include "SPIRIT/Logging/AnsiEscape.hpp"

TEST_CASE("Ansi Escapes")
        SECTION("Ansi Sequence Detection")
        {
            REQUIRE(sp::traits::isAnsiEscape<sp::AnsiEscape>::value);
            REQUIRE(sp::traits::isAnsiEscape<sp::FgColor>::value);
            REQUIRE(sp::traits::isAnsiEscape<sp::BgColor>::value);
            REQUIRE(sp::traits::isAnsiEscape<sp::AnsiStyle>::value);
            REQUIRE(sp::traits::isAnsiEscape<sp::RgbFgColor>::value);

            REQUIRE(sp::traits::isAnsiEscape<const sp::AnsiEscape &>::value);
            REQUIRE(sp::traits::isAnsiEscape<const sp::FgColor &>::value);
            REQUIRE(sp::traits::isAnsiEscape<const sp::BgColor &>::value);
            REQUIRE(sp::traits::isAnsiEscape<const sp::AnsiStyle &>::value);

            REQUIRE(sp::traits::isAnsiEscape<volatile sp::AnsiEscape &&>::value
            );
            REQUIRE(sp::traits::isAnsiEscape<volatile sp::FgColor &&>::value);
            REQUIRE(sp::traits::isAnsiEscape<volatile sp::BgColor &&>::value);
            REQUIRE(sp::traits::isAnsiEscape<volatile sp::AnsiStyle &&>::value);

            REQUIRE_FALSE(sp::traits::isAnsiEscape<int>::value);
            REQUIRE_FALSE(sp::traits::isAnsiEscape<const char *>::value);
            REQUIRE_FALSE(sp::traits::isAnsiEscape<unsigned char &>::value);
            REQUIRE_FALSE(sp::traits::isAnsiEscape<float>::value);
            REQUIRE_FALSE(sp::traits::isAnsiEscape<volatile std::string>::value
            );
            REQUIRE_FALSE(sp::traits::isAnsiEscape<void>::value);
            REQUIRE_FALSE(sp::traits::isAnsiEscape<void *>::value);
        }
