#include "celero/Celero.h"

#define SPIRIT_VERBOSE 1
#include "SPIRIT/Base.hpp"

CELERO_MAIN


////////////////////////////////////////////////////////////
// Benchmark of the overhead for StreamSinks when needing
// to filer out ansi escape sequences
////////////////////////////////////////////////////////////


sp::LoggerPtr withColors
    = sp::makeLogger<sp::AnsiFileSink_st>("Y Colors", stdout, sp::ansiMode::always);

sp::LoggerPtr noColors
    = sp::makeLogger<sp::AnsiFileSink_st>("N Colors", stdout, sp::ansiMode::never);


////////////////////////////////////////////////////////////
// Constant String
////////////////////////////////////////////////////////////

class Fixture : public celero::TestFixture
{
public:

    static constexpr std::size_t nMessages = 25;

    static constexpr std::size_t nColors = 9;
    static constexpr sp::FgColor colors[nColors]{
        sp::black,
        sp::red,
        sp::green,
        sp::yellow,
        sp::blue,
        sp::magenta,
        sp::cyan,
        sp::white,
        sp::defaultFg};

    Fixture() {}

    virtual std::vector<celero::TestFixture::ExperimentValue>
    getExperimentValues() const override
    {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace;

        for (int i = 1; i < nMessages; i+=5)
        {
            problemSpace.push_back({i, 1000});
        }

        return problemSpace;
    }

    virtual void
    setUp(const celero::TestFixture::ExperimentValue & experimentValue)
    {
        std::size_t n = experimentValue.Value;

        std::stringstream msg{};
        for (std::size_t i = 0; i < n; ++i)
        {
            msg << colors[i % nColors]
                << "This is some pretty long text to reduce the influence of "
                   "having less content to print out when ansiSequences are "
                   "filtered out.";
        }
        msg << sp::reset;
        current = msg.str();
    }

    virtual void
    tearDown() override
    {
    }

    std::string current;
};


// does not parse to filter
BASELINE_F(AnsiParsing, WithColors, Fixture, 30, 0)
{
    *withColors << sp::Info(this->current);
}

// Parses to remove them
BENCHMARK_F(AnsiParsing, NoColors, Fixture, 30, 0)
{
    *noColors << sp::Info(this->current);
}
