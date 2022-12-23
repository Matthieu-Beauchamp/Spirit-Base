#include <list>
#include <thread>
#include <vector>

#define SPIRIT_VERBOSE
#include "SPIRIT/Core.hpp"

// TODO: Not updated
using namespace sp;

struct MovingColors
{
    MovingColors(
        FgColor base,
        const std::vector<FgColor> & movingColors,
        sp::Uint32 nColors
    )
    {
        sp::Uint32 i = 0;
        while (i < nColors)
        {
            for (const auto & col : movingColors)
            {
                if (i++ < nColors)
                    colors.push_back(base);
                if (i++ < nColors)
                    colors.push_back(col);
            }
        }
    }

    const std::list<FgColor> &
    next()
    {
        auto col = colors.back();
        colors.pop_back();
        colors.push_front(col);
        return colors;
    }

private:

    std::list<FgColor> colors{};
};


int
main()
{
    std::initializer_list<AnsiStyle> mods = {italic, bold};
    std::vector<std::string> words
        = {"The ",
           "Core ",
           "of ",
           "Spirit ",
           "Engine ",
           "is ",
           "up ",
           "and ",
           "running",
           "!"};

    MovingColors colors{
        green,
        {magenta, cyan},
        (sp::Uint32)words.size()
    };


    for (int i = 0; i < 5; ++i)
    {
        std::vector<std::string> coloredWords{};
        const auto & cols = colors.next();
        auto c            = cols.begin();

        for (int j = 0; j < words.size(); ++j)
            coloredWords.push_back(modify(colorFg(words[j], *c++, false), mods));

        std::string msg = "";
        for (auto && w : coloredWords) msg += std::move(w);

        // SPIRIT_INFO(msg);
        sp::ansiOut << msg << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds{200});
    }

    // try
    // {
    //     SPIRIT_THROW(sp::SpiritError(1)); // printable object
    // }
    // catch (sp::SpiritError & e)
    // {
    //     try
    //     {
    //         SPIRIT_THROW(sp::SpiritError(
    //             "Format in {} constructor is working: {}",
    //             "Error",
    //             bool(1)
    //         ));
    //     }
    //     catch (sp::SpiritError & e)
    //     {
    //     }
    // }

    return 0;
}