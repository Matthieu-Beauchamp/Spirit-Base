#include "celero/Celero.h"

#include "SPIRIT/Base.hpp"
#include "spdlog/sinks/ansicolor_sink.h"

CELERO_MAIN


////////////////////////////////////////////////////////////
// Benchmark of streaming messages instead of using macros
//
// Groups:
//  - Constant String: String only and known at compile time with n: message length
//  - String: String only, not known at compile time with n: message length
//  - Constant Format String: Format string known at compile time with n: number of format arg
//  - Format String: Format string not known at compile time with n: number of format arg
////////////////////////////////////////////////////////////

// TODO: Format string benchmarks,
//  probably similar...


// use a minimal logger to minimize other computations
sp::LoggerPtr benchLogger
    = sp::makeLogger<spdlog::sinks::ansicolor_stdout_sink_st>("Benchmark");


#define LOG_MACRO(logger, ...)                                                 \
    SPDLOG_LOGGER_CALL(logger, sp::LogLevel::info, __VA_ARGS__)


////////////////////////////////////////////////////////////
// Constant String
////////////////////////////////////////////////////////////

class ConstStrFixture : public celero::TestFixture
{
public:

    static constexpr std::size_t nMessages = 5;
    static constexpr const char * strs[nMessages]{
        "",
        "Hello",
        "This is a benchmark",
        "I truly have a lot of inspiration for this meaningful message that is "
        "destined to you. \nBest regards,\nSpirit",

        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean nec "
        "placerat dolor. Proin quis nulla a lectus suscipit rutrum. Ut "
        "vestibulum ipsum a metus sollicitudin luctus. Aliquam sed tincidunt "
        "mauris. Etiam congue, nibh tincidunt sagittis porttitor, libero quam "
        "luctus ligula, dictum tempus arcu arcu vel elit. Aliquam vel est et "
        "ipsum cursus consectetur. Etiam a velit maximus, placerat sem in, "
        "feugiat enim. Donec fringilla in mi feugiat aliquam. Nullam nec porta "
        "dui. Duis eget placerat lorem, non mattis urna. Maecenas tristique "
        "non enim sit amet lobortis. In efficitur rhoncus tincidunt. Aenean at "
        "neque id diam scelerisque accumsan porta ut quam. Nullam ultricies "
        "eget mi sit amet cursus. Vivamus accumsan, dui id efficitur "
        "vulputate, neque purus dapibus quam, a condimentum lorem dui sit amet "
        "felis. Nunc pulvinar, sem a luctus suscipit, enim arcu dignissim "
        "dolor, facilisis semper enim mauris ac velit. ",
    };


    ConstStrFixture() {}

    virtual std::vector<celero::TestFixture::ExperimentValue>
    getExperimentValues() const override
    {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace;

        for (int i = 0; i < nMessages; i++)
        {
            std::size_t n = std::strlen(strs[i]);
            problemSpace.push_back({n, 10000 / (n + 1)});
        }

        return problemSpace;
    }

    virtual void
    setUp(const celero::TestFixture::ExperimentValue & experimentValue)
    {
        benchLogger->set_level(sp::LogLevel::trace);
        benchLogger->set_pattern("%v");

        current = whichIsCurrent(experimentValue.Value);
    }

    std::size_t
    whichIsCurrent(std::size_t size)
    {
        for (std::size_t i = 0; i < nMessages; ++i)
        {
            if (strlen(strs[i]) == size)
            {
                return i;
            }
        }

        return 0;
    }

    virtual void
    tearDown() override
    {
    }

    std::size_t current;
};


BASELINE_F(ConstantString, Macro, ConstStrFixture, 30, 0)
{
    LOG_MACRO(benchLogger, this->strs[this->current]);
}

BENCHMARK_F(ConstantString, StreamMessage, ConstStrFixture, 30, 0)
{
    *benchLogger << sp::Info{this->strs[this->current]};
}


////////////////////////////////////////////////////////////
// String
////////////////////////////////////////////////////////////

class StrFixture : public celero::TestFixture
{
public:

    static constexpr std::size_t nMessages = 6;

    StrFixture() {}

    virtual std::vector<celero::TestFixture::ExperimentValue>
    getExperimentValues() const override
    {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace;

        for (int i = 0; i < nMessages; i++)
        {
            std::size_t n = (2) << (i + 1);
            problemSpace.push_back({n, 10000 / (n)});
        }

        return problemSpace;
    }

    virtual void
    setUp(const celero::TestFixture::ExperimentValue & experimentValue)
    {
        benchLogger->set_level(sp::LogLevel::trace);
        benchLogger->set_pattern("%v");

        str.resize(experimentValue.Value);
        for (char & c : str) c = randChar();
    }

    char
    randChar() const
    {
        int c = std::rand() & 0x7F;
        c     = std::max(c, 32);
        c     = std::min(c, 122); // don't let '{' or '}' in string
        return (char)c;
    }

    virtual void
    tearDown() override
    {
    }

    std::string str;
};


BASELINE_F(String, Macro, StrFixture, 30, 0)
{
    LOG_MACRO(benchLogger, this->str);
}

BENCHMARK_F(String, StreamMessage, StrFixture, 30, 0)
{
    *benchLogger << sp::Info{this->str};
}
