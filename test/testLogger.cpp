#include "SPIRIT/Logging/Logger.hpp"
#include "catch2/catch_test_macros.hpp"

struct UserDefined
{
    friend std::ostream &
    operator<<(std::ostream & os, UserDefined u)
    {
        return os << "FOO";
    }
};

TEST_CASE("Messages")
{
    SECTION("Compile time string output")
    {
        sp::Trace tr{"compile time string #{}", 1};
        REQUIRE(tr.str() == "compile time string #1");

        sp::Debug deb{"user: {}", UserDefined{}};
        REQUIRE(deb.str() == "user: FOO");

        sp::Info info{UserDefined{}};
        REQUIRE(info.str() == "FOO");

        sp::Warn warn{"BAR"};
        REQUIRE(warn.str() == "BAR");

        sp::Error err{};
        REQUIRE(err.str() == "");
    }

    SECTION("Run time string output")
    {
        std::string fmt{"pi: {}"};
        float pi = 3.1416;

        sp::Critical critical{fmt, pi};
        REQUIRE(critical.str() == "pi: 3.1416");
    }

    SECTION("Source location")
    {
        REQUIRE(sp::Info{}.sourceLoc().line() == __LINE__);
        REQUIRE(sp::Info{}.sourceLoc().function_name() == __FUNCTION__);
        REQUIRE(std::string{sp::Info{}.sourceLoc().file_name()} == __FILE__);
    }
}


bool
containsAnsiSequence(const std::string & str)
{
    return std::find(str.begin(), str.end(), sp::AnsiEscape::ESC) != str.end();
}

TEST_CASE("StreamSinks")
{
    SECTION("Sequence Filtering")
    {
        auto on  = std::make_shared<sp::StreamSink_mt<std::stringstream>>(true);
        auto off = std::make_shared<sp::StreamSink_mt<std::stringstream>>(false);

        spdlog::logger logger{
            "Logger",
            {on, off}
        };

        logger << sp::Info{"{}hello{}", sp::green, sp::reset};

        REQUIRE(containsAnsiSequence(on->stream().str()) == true);
        REQUIRE_FALSE(containsAnsiSequence(off->stream().str()) == true);
    }

    // should test proper integration with spdlog's API...
}

TEST_CASE("File Sinks")
{
    SECTION("Construction")
    {
        SECTION("Automatic")
        {
            sp::FileSink_mt out{stdout};
            REQUIRE(out.file() == stdout);
            REQUIRE(out.isAnsiEnabled() == sp::supportsAnsi(stdout));
        }

        SECTION("Always")
        {
            sp::FileSink_mt out{stdout, sp::FileSink_mt::always};
            REQUIRE(out.file() == stdout);
            REQUIRE(out.isAnsiEnabled() == true);
        }

        SECTION("Never")
        {
            sp::FileSink_mt out{stdout, sp::FileSink_mt::never};
            REQUIRE(out.file() == stdout);
            REQUIRE(out.isAnsiEnabled() == false);
        }

        SECTION("Pattern formatter")
        {
            auto sink
                = std::make_shared<sp::StreamSink_mt<std::stringstream>>(false);
            spdlog::logger logger{"Logger", sink};

            // Note that spdlog adds a newline after each message
            logger.set_pattern("[%n][%l]\n%v");
            logger << sp::Info{"An interesting message"};
            REQUIRE(
                sink->stream().str() == "[Logger][info]\nAn interesting message\n"
            );

            sink->stream().str("");
            logger.set_pattern("%v");
            logger << sp::Critical{"Danger!"};
            REQUIRE(
                sink->stream().str() == "Danger!\n"
            );
        }
    }

    // Should test that pattern formatters and integration with spdlog's api
}

TEST_CASE("Spirit's Logger"){
    sp::LoggerPtr logger = sp::spiritLogger();

    auto onSink = std::make_shared<sp::StreamSink_mt<std::stringstream>>(true);
    auto offSink = std::make_shared<sp::StreamSink_mt<std::stringstream>>(false);

    logger->sinks().push_back(onSink);
    logger->sinks().push_back(offSink);
    sp::spiritLogger()->set_pattern(sp::spiritPattern());

    *logger << sp::Info{"Meaningful information: {}{}{}", sp::red, 1, sp::reset};
    sp::spiritLog() << sp::Warn("Boom");
    logger->flush();

    std::string onOut{onSink->stream().str()};
    std::string offOut{offSink->stream().str()};

    sp::spiritLog() << sp::Info{onOut};
    sp::spiritLog() << sp::Info{offOut};

    REQUIRE(onOut != "");
    REQUIRE(offOut != "");
    REQUIRE(containsAnsiSequence(onOut) == true);
    REQUIRE(containsAnsiSequence(offOut) == false);

}