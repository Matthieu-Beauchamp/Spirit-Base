#include "SPIRIT/Base.hpp"

#include <thread>

class Worker
{
public:

    Worker(float progressRate) : rate{progressRate} {}

    float
    doWork()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        progress += rate;
        progress = progress > 1 ? 1 : progress;
        return progress;
    }

private:

    float rate;
    float progress = 0.0;
};


class Loader
{
public:

    Loader(Worker worker) : worker{worker} {}

    void
    load()
    {
        sp::Clock clk{std::chrono::milliseconds{200}};

        float progress = 0.0;
        int percent    = 0;
        printProgress(percent); // firstLine

        while (percent < 100)
        {
            progress = worker.doWork();
            percent  = progress * 100;

            // Avoids updating too fast (glitching display)
            if (clk.getCurrentDt() > clk.getMinimumTickPeriod())
            {
                clk.tick();
                removeLine();
                printProgress(percent);
            }
        }
        removeLine();
        printProgress(percent);
    }

private:

    void
    printProgress(int percent)
    {
        sp::ansiOut << sp::format("{:>3}% |", percent);

        if (percent % 2 == 1)
        {
            percent++;
        } // otherwise a block will be missing

        sp::ansiOut << sp::green;
        for (int i = 0; i < percent / 2; ++i) sp::ansiOut << "X";

        // You can also use a colored background and print spaces to simulate blocks,
        // sp::ansiOut << sp::onGreen;
        // for (int i = 0; i < percent; ++i) sp::ansiOut << " ";

        // or use the extended Ascii / utf block characters if they are available on your machine

        sp::ansiOut << sp::onDefault << sp::defaultFg;

        for (int i = 0; i < (100 - percent) / 2; ++i) sp::ansiOut << "-";
        sp::ansiOut << "| ";

        loadingSymbol(percent);
    }

    void
    removeLine()
    {
        sp::ansiOut << sp::CursorUp{1} << sp::EraseLine{} << sp::CarriageRet{};
    }

    void
    loadingSymbol(int percent)
    {
        if (percent == 100)
        {
            return;
        }

        // static variables allows keeping state between loaders
        static char symbols[] = {'-', '\\', '|', '/'};
        static int mode       = 0;

        sp::ansiOut << symbols[mode] << " " << std::endl;
        mode = (mode + 1) % 4;
    }

    Worker worker;
};


int
main(int argc, char ** argv)
{
    // Allows forcing to files to see if AnsiSequences are correctly ignored
    // Allows forcing to files to see if AnsiSequences are correctly ignored
    if (!(argc == 2 && std::string{argv[1]} == "-f"))
        if (!sp::ansiOut.isAnsiEnabled())
            throw sp::SpiritError{"This example must be run in a terminal with "
                                  "Ansi escape support!"};

    for (int i = 0; i < 10; ++i)
    {
        Loader l{Worker{0.0015f * (i + 1)}};
        l.load();
        sp::ansiOut << std::endl;
    }
    return 0;
}