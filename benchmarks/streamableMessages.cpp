#include "celero/Celero.h"

#include "SPIRIT/Base.hpp"

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

#define LOG_MACRO(logger, ...)                                                 \
    SPDLOG_LOGGER_CALL(logger, sp::LogLevel::info, __VA_ARGS__)


////////////////////////////////////////////////////////////
// Constant String
////////////////////////////////////////////////////////////

class ConstStrFixture : public celero::TestFixture
{
public:

    static constexpr std::size_t nMessages = 6;
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
        "dolor, facilisis semper enim mauris ac velit."
        "Fusce tempor, lectus congue ultricies ultrices, nulla velit blandit "
        "erat, vel feugiat sem quam vel diam. Nulla rhoncus auctor iaculis. "
        "Integer congue enim eget sodales vehicula. Donec facilisis malesuada "
        "libero in dignissim. Maecenas posuere venenatis dignissim. Curabitur "
        "faucibus justo ut tempus congue. Nunc molestie interdum porta. Sed "
        "ullamcorper neque ac egestas tristique. Nunc sit amet diam euismod "
        "turpis ullamcorper aliquet a vitae ligula. Ut faucibus, nulla vel "
        "suscipit congue, ex enim pulvinar erat, sit amet aliquam est nisi non "
        "elit. Morbi pharetra et augue eu sollicitudin. Proin consectetur arcu "
        "sed pulvinar cursus. Donec rhoncus, urna id mattis maximus, neque "
        "neque tristique elit, et gravida velit ante a lectus. Pellentesque "
        "vel bibendum quam."
        "Nunc sodales imperdiet tellus nec efficitur. Sed id molestie neque. "
        "Donec eu mattis justo, eget hendrerit nisi. Sed venenatis vel odio "
        "eget finibus. Pellentesque ligula nunc, convallis non justo "
        "condimentum, posuere tincidunt quam. Nullam ultrices mi quis bibendum "
        "mattis. Vestibulum bibendum, massa et ultrices commodo, leo ipsum "
        "maximus metus, at bibendum orci nisl eu sem. Vivamus sagittis lectus "
        "orci, sit amet maximus enim pellentesque ut. Mauris quis consequat "
        "nisl, ut lacinia turpis. Nam erat urna, sagittis eu urna vitae, "
        "congue consequat purus."
        "Suspendisse potenti. Etiam non accumsan libero. Nunc venenatis mauris "
        "nunc, a ultricies eros accumsan nec. Praesent ex orci, faucibus ut "
        "mauris eget, auctor vestibulum nisl. Nulla facilisi. Nulla arcu "
        "lacus, malesuada vel laoreet vitae, vulputate et eros. Etiam ex "
        "mauris, tristique non libero ac, imperdiet scelerisque odio. Nunc "
        "malesuada tortor quis aliquam porttitor. Ut pretium consectetur "
        "suscipit. Morbi mauris purus, rutrum quis justo eget, fringilla "
        "luctus mi. Morbi mi dui, consectetur nec enim a, eleifend molestie "
        "magna. Nullam condimentum erat nec nulla volutpat tincidunt."
        "In nec sapien aliquam, hendrerit enim vitae, tempor turpis. "
        "Vestibulum in imperdiet lectus, at tristique diam. Ut quis purus "
        "euismod, rhoncus lorem vitae, posuere eros. Mauris in efficitur "
        "lorem, sit amet tempus arcu. Suspendisse potenti. Phasellus mattis mi "
        "dignissim euismod euismod. Donec scelerisque lacus lectus, non "
        "sodales erat pretium eget. Proin consectetur arcu non tellus "
        "imperdiet finibus. Fusce scelerisque metus lorem, eu facilisis lorem "
        "pellentesque ac. "};


    ConstStrFixture() {}

    virtual std::vector<celero::TestFixture::ExperimentValue>
    getExperimentValues() const override
    {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace;

        for (int i = 0; i < nMessages; i++)
        {
            std::size_t n = std::strlen(strs[i]);
            problemSpace.push_back({n, 1000});
        }

        return problemSpace;
    }

    virtual void
    setUp(const celero::TestFixture::ExperimentValue & experimentValue)
    {
        logger = sp::spiritLogger(); // ensure logger is initialized
        logger->set_level(sp::LogLevel::trace);
        current = 3;
    }

    virtual void
    tearDown() override
    {
    }

    sp::LoggerPtr logger;
    std::size_t current;
};


BASELINE_F(ConstantString, Macro, ConstStrFixture, 30, 0)
{
    LOG_MACRO(this->logger, this->strs[this->current]);
}

BENCHMARK_F(ConstantString, StreamMessage, ConstStrFixture, 30, 0)
{
    *this->logger << sp::Info { this->strs[this->current] };
}

