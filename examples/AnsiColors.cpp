#include "SPIRIT/Base.hpp"

int
main(int argc, char ** argv)
{
    // Allows forcing to files to see if AnsiSequences are correctly ignored
    if (!(argc == 2 && std::string{argv[1]} == "-f"))
        if (!sp::ansiOut.isAnsiEnabled())
            throw sp::SpiritError{"This example must be run in a terminal with "
                                  "Ansi escape support!"};

    std::array<sp::FgColor, 9> fgs{
        sp::black,
        sp::red,
        sp::green,
        sp::yellow,
        sp::blue,
        sp::magenta,
        sp::cyan,
        sp::white,
        sp::defaultFg};
    for (sp::FgColor fg : fgs) { sp::ansiOut << fg << "AaBbCc\n"; }

    std::array<sp::BgColor, 9> bgs{
        sp::onBlack,
        sp::onRed,
        sp::onGreen,
        sp::onYellow,
        sp::onBlue,
        sp::onMagenta,
        sp::onCyan,
        sp::onWhite,
        sp::onDefault};

    for (sp::BgColor bg : bgs)
        sp::ansiOut << bg << "         " << sp::onDefault << "\n";


    // some styles omitted
    std::array<sp::Style, 5> styles{
        sp::reset, // no style
        sp::bold,
        sp::underline,
        sp::swapColors,
        sp::crossed};

    for (sp::Style s : styles)
        sp::ansiOut << s << "abcdefghijklmnop\n\n" << sp::reset;

    sp::ansiOut << std::endl;
    return 0;
}