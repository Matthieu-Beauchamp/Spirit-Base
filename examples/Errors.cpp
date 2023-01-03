#include "SPIRIT/Base.hpp"

int
main()
{
    try
    {
        throw sp::SpiritError{"Oh no! {} != {}", 1, 2};
    }
    catch (std::exception & e)
    {
        std::cout << e.what();
    }

    return 0;
}