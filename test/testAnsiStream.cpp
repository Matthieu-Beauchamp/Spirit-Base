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

// TODO: Many tests using "child" stringstream were removed,
//      now very little is tested here.
// -output to files should not be colored when automatic
// -test correct terminal detection..?
// -test always and never modes with streamstreams
// -test ansiMode changes.
//

TEST_CASE("Ansi Stream on connex classes")
{
    SECTION("Concepts")
    {
        SECTION("isStream")
        {
            REQUIRE(sp::traits::isStream<sp::AnsiFileStream>::value);
            REQUIRE(
                sp::traits::isStream<sp::AnsiStreamWrapper<std::ostream>>::value
            );
            REQUIRE(
                sp::traits::isStream<sp::AnsiStreamWrapper<std::stringstream>>::value
            );

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

        SECTION("Wrapped type")
        {
            REQUIRE(std::is_same<
                    sp::traits::AnsiWrapped_t<std::ostream>,
                    sp::AnsiStreamWrapper<std::ostream>>::value);

            // does not wrap twice
            REQUIRE(std::is_same<
                    sp::traits::AnsiWrapped_t<sp::AnsiStreamWrapper<std::ostream>>,
                    sp::AnsiStreamWrapper<std::ostream>>::value);
        }
    }

    SECTION("Ansi Stream construction")
    {
        sp::AnsiFileStream original{stdout};
        auto f           = original.stream().file();
        bool ansiEnabled = original.isAnsiEnabled();
        REQUIRE(f != nullptr);

        // Move constructor was disabled, standard ostreams differ too much to
        // generalize it sp::AnsiFileStream last{std::move(original)}; REQUIRE(last.file()
        // == f); REQUIRE(last.isAnsiEnabled() == ansiEnabled);
    }

    SECTION("Class Interface")
    {
        sp::AnsiFileStream out{stdout};

        bool allowsSequences = out.isAnsiEnabled();

        // Equivalent access to the underlying stream, operator-> is given as
        // a shorthand since we could not implement the interface using inheritance.
        REQUIRE(&out.stream() == out.operator->());

        // Same as above, outPtr-> does not jump to underlying stream, points to AnsiFileStream.
        sp::AnsiFileStream * outPtr = &out;
        REQUIRE(&outPtr->stream() == outPtr->operator->());

        // Should not compile, we want to block redirections since
        // we cannot know when they happen and disable ansi if needed.
        // (It can still be accessed by static casting to underlying stream type
        // but then we did all we could)
        // TODO: meta programming magic here to test that its inacessible...
        // REQUIRE(out.stream().rdbuf());
        // REQUIRE(out->rdbuf());
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