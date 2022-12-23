#include "SPIRIT/Logging/AnsiSequence.hpp"
#include "SPIRIT/Logging/AnsiStream.hpp"
#include "catch2/catch_all.hpp"

#include <iomanip>
#include <ostream>
#include <iosfwd>
struct UserDefined
{
    float x;

    friend std::ostream &
    operator<<(std::ostream & os, UserDefined user)
    {
        return os << "UserDefined: " << user.x;
    }
};

struct NotPrintable
{
};

// TODO: Input is never tested
// TODO: Printable and isStream concept checks

TEST_CASE("Ansi Stream on connex classes")
{
    SECTION("Concepts")
    {
        SECTION("Ansi Sequence Detection")
        {
            REQUIRE(sp::traits::isAnsiSequence<sp::AnsiSequence>::value);
            REQUIRE(sp::traits::isAnsiSequence<sp::FgColor>::value);
            REQUIRE(sp::traits::isAnsiSequence<sp::BgColor>::value);
            REQUIRE(sp::traits::isAnsiSequence<sp::AnsiStyle>::value);
            REQUIRE(sp::traits::isAnsiSequence<sp::RgbFgColor>::value);

            REQUIRE(sp::traits::isAnsiSequence<const sp::AnsiSequence &>::value);
            REQUIRE(sp::traits::isAnsiSequence<const sp::FgColor &>::value);
            REQUIRE(sp::traits::isAnsiSequence<const sp::BgColor &>::value);
            REQUIRE(sp::traits::isAnsiSequence<const sp::AnsiStyle &>::value);

            REQUIRE(sp::traits::isAnsiSequence<volatile sp::AnsiSequence &&>::value
            );
            REQUIRE(sp::traits::isAnsiSequence<volatile sp::FgColor &&>::value);
            REQUIRE(sp::traits::isAnsiSequence<volatile sp::BgColor &&>::value);
            REQUIRE(sp::traits::isAnsiSequence<volatile sp::AnsiStyle &&>::value);

            REQUIRE_FALSE(sp::traits::isAnsiSequence<int>::value);
            REQUIRE_FALSE(sp::traits::isAnsiSequence<const char *>::value);
            REQUIRE_FALSE(sp::traits::isAnsiSequence<unsigned char &>::value);
            REQUIRE_FALSE(sp::traits::isAnsiSequence<float>::value);
            REQUIRE_FALSE(sp::traits::isAnsiSequence<volatile std::string>::value
            );
            REQUIRE_FALSE(sp::traits::isAnsiSequence<void>::value);
            REQUIRE_FALSE(sp::traits::isAnsiSequence<void *>::value);
        }

        SECTION("isStream")
        {
            REQUIRE(sp::traits::isStream<sp::AnsiStream>::value);
            REQUIRE(sp::traits::isStream<sp::details::AnsiStreamWrapper<std::ostream>>::value);
            REQUIRE(sp::traits::isStream<sp::details::AnsiStreamWrapper<std::stringstream>>::value);

            REQUIRE(sp::traits::isStream<std::ostream>::value);
            REQUIRE(sp::traits::isStream<std::basic_iostream<char>>::value);
            REQUIRE(sp::traits::isStream<std::basic_iostream<wchar_t>>::value);
            REQUIRE(sp::traits::isStream<std::basic_stringstream<char>>::value);

            REQUIRE_FALSE(sp::traits::isStream<std::string>::value);
            REQUIRE_FALSE(sp::traits::isStream<UserDefined>::value);
            REQUIRE_FALSE(sp::traits::isStream<NotPrintable>::value);
            REQUIRE_FALSE(sp::traits::isStream<int>::value);
        }

        SECTION("Printable")
        {
            REQUIRE(sp::traits::Printable<int>::value);
            REQUIRE(sp::traits::Printable<float>::value);
            REQUIRE(sp::traits::Printable<UserDefined>::value);
            REQUIRE(sp::traits::Printable<std::string>::value);

            REQUIRE(sp::traits::Printable<const int &>::value);
            REQUIRE(sp::traits::Printable<const float &>::value);
            REQUIRE(sp::traits::Printable<const UserDefined &>::value);
            REQUIRE(sp::traits::Printable<const std::string &>::value);

            REQUIRE_FALSE(sp::traits::Printable<NotPrintable>::value);
            REQUIRE_FALSE(sp::traits::Printable<std::ostream>::value);
            REQUIRE_FALSE(sp::traits::Printable<const NotPrintable &>::value);
            REQUIRE_FALSE(sp::traits::Printable<const std::ostream &>::value);
        }
    }


    SECTION("Ansi Stream construction")
    {
        sp::AnsiStream original{stdout};
        auto f           = original.file();
        bool ansiEnabled = original.isAnsiEnabled();
        REQUIRE(f != nullptr);

        sp::AnsiStream last{std::move(original)};
        REQUIRE(last.file() == f);
        REQUIRE(last.isAnsiEnabled() == ansiEnabled);
    }

    SECTION("Child Streams")
    {
        sp::AnsiStream on{stdout, sp::AnsiStream::always};
        sp::AnsiStream off{stdout, sp::AnsiStream::never};

        // Child streams
        auto ss1 = on.makeStringStream();
        auto ss2 = off.makeStringStream();

        std::stringstream expectedOn{};
        expectedOn << sp::red << "hello";
        std::stringstream expectedOff{};
        expectedOff << "world";

        ss1 << sp::red << "hello";
        ss2 << "world";
        std::swap(ss1, ss2);

        // note that they also use the .stream() or operator-> to access the
        // underlying stream
        REQUIRE(ss2->str() == expectedOn.str());
        REQUIRE(ss1->str() == expectedOff.str());
    }


    SECTION("Class Interface")
    {
        sp::AnsiStream out{stdout};

        bool allowsSequences = out.isAnsiEnabled();
        auto childStream     = out.makeStringStream();

        // AnsiStream can create stream which will enable/disable
        // AnsiSequence according to the AnsiStream's status
        // (ie is in terminal and colors are supported)
        REQUIRE(childStream.isAnsiEnabled() == allowsSequences);

        // Equivalent access to the underlying stream, operator-> is given as
        // a shorthand since we could not implement the interface using inheritance.
        REQUIRE(&out.stream() == out.operator->());

        // Same as above, outPtr-> does not jump to underlying stream, points to AnsiStream.
        sp::AnsiStream * outPtr = &out;
        REQUIRE(&outPtr->stream() == outPtr->operator->());

        // Should not compile, we want to block redirections since
        // we cannot know when they happen and disable ansi if needed.
        // (It can still be accessed by static casting to underlying stream type
        // but then we did all we could)
        // TODO: meta programming magic here to test that its inacessible...
        // REQUIRE(out.stream().rdbuf());
        // REQUIRE(out->rdbuf());
    }

    SECTION("Ansi stringstream")
    {
        SECTION("With Ansi")
        {
            // TODO: Static method for building those without creating an
            // AnsiStream would be useful
            sp::AnsiStream EscapeOn{stdout, sp::AnsiStream::always};
            auto ansiSs = EscapeOn.makeStringStream();
            std::stringstream ss{};

            // normal streams always receive sequences
            ansiSs << sp::red << "hello " << 1.0 << sp::reset;
            ss << sp::red << "hello " << 1.0 << sp::reset;

            REQUIRE(ansiSs->str() == ss.str());
        }

        SECTION("No Ansi")
        {
            // TODO: Static method for building those without creating an
            // AnsiStream would be useful
            sp::AnsiStream EscapeOff{stdout, sp::AnsiStream::never};
            auto noAnsi = EscapeOff.makeStringStream();
            std::stringstream ss{};

            noAnsi << sp::red << "hello" << sp::onBlack << sp::italic << "!";
            ss << "hello!";

            REQUIRE(noAnsi.stream().str() == ss.str());
        }
    }


    SECTION("ansiOut")
    {
        // TODO: Terminal vs File detection and checking of terminal
        // capabilities would probably require .sh and .bat scripts or manual checks

        sp::ansiOut << sp::cyan << sp::onWhite << "ansiOut" << sp::reset
                    << std::endl;
        sp::ansiOut << 1 << sp::red << " " << 2.0 << " " << sp::black
                    << sp::onYellow << UserDefined{3.1416} << sp::reset
                    << std::endl
                    << "Formatting looks ok?\n"
                    << std::endl;

        sp::ansiErr << sp::cyan << sp::onWhite << "ansiErr" << sp::defaultFg
                    << sp::onDefault << std::endl;
        sp::ansiErr << 1 << sp::red << " " << std::setprecision(2) << std::fixed
                    << 2.0 << " " << sp::black << sp::onYellow
                    << UserDefined{3.1416} << sp::reset << std::endl
                    << "Formatting looks ok?\n";
        sp::ansiErr->flush();

        REQUIRE(true);
    }
}