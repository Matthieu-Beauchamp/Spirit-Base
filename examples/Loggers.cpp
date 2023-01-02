// enable all log levels
#define SPIRIT_VERBOSE 1

#include "SPIRIT/Base.hpp"

int
main()
{
    sp::LoggerPtr spLogger = sp::spiritLogger();

    // create a logger
    sp::LoggerPtr myLogger = sp::makeLogger("User Logger");

    // Create custom ansi (color/textstyle) aware sinks
    auto noColors
        = std::make_shared<sp::AnsiStreamSink_mt<std::stringstream>>(false);

    auto withColors
        = std::make_shared<sp::AnsiStreamSink_mt<std::stringstream>>(true);
    
    auto maybeColors
        = std::make_shared<sp::AnsiFileSink_mt>(stdout, sp::ansiMode::automatic);

    // make my logger use the newly created sinks for output
    myLogger->sinks().push_back(noColors);
    myLogger->sinks().push_back(withColors);
    myLogger->sinks().push_back(maybeColors);

    spLogger->sinks() = myLogger->sinks(); // spirit will output to the same sinks

    // change formatting pattern to "[name]: msg"
    // Note that spdlog will append newlines after messages.
    myLogger->set_pattern("[%n]: %v");
    spLogger->set_pattern("[%n]: %v");


    *spLogger << sp::Info{"{}Spirit outputs here{}", sp::green, sp::reset};
    *myLogger << sp::Warn{"In the same place as the {}{}user{}", sp::cyan, sp::bold, sp::reset};

    // AnsiStreamSink allows accessing underlying stream
    std::cout << "\nWithout colors: \n" << noColors->stream().str() << "\n";
    std::cout << "With colors: \n" << withColors->stream().str() << "\n";

    return 0;
}