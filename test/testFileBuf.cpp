#include "SPIRIT/Base/Logging/details/FileBuf.hpp"
#include "catch2/catch_all.hpp"

#include <stdio.h>
#include <string>
#include <vector>

// TODO: This works fine, there is another problem.
//  when writing '\n':
//      1. fileBuf puts '\n', windows appends a '\r'
//      2. on the next write, we overwrite '\r' because we seek to where we
//          think we are, ie on '\r'
template <class char_type>
std::size_t
outputSize(const std::basic_string<char_type> & str)
{
    if (str.size() == 0)
        return 0;

#if defined(SPIRIT_OS_WINDOWS)
    return str.size() + std::count(str.begin(), str.end(), '\n');
#endif

    return str.size();
}


TEST_CASE("basic_streambuf behavior of FileBuffer", "[FileBuffer]")
{
    // todo: make constants for test variables (ie string, its size, ...)

    SECTION("Construction")
    {
        FILE * f = fopen("tempConstr.txt", "w");
        // Note that Filebuf does not open nor close any FILE ever
        sp::details::IOFileBuf filebuf{f};

        REQUIRE(filebuf.file() == f);

        fclose(f);
    }


    constexpr int nRepeats = 1;

    std::string allChars{};
    for (char i = 0x20; i < 0x7F; ++i) { allChars.push_back(i); }

    std::string nullCharInside{};
    nullCharInside.push_back('a');
    nullCharInside.push_back('\0');
    nullCharInside.push_back('b');

    std::vector<std::string> strs{"Hello", nullCharInside, "", "\n", allChars};
    std::string allStrings{};
    for (const auto & s : strs) allStrings += s;


    // When each string is repeated
    std::string expected{};
    for (const auto & str : strs)
    {
        for (int _ = 0; _ < nRepeats; ++_) { expected += str; };
    };

    SECTION("Output")
    {
        FILE * f = fopen("tempOut.txt", "w+");

        sp::details::OutFileBuf filebuf{f};
        for (std::string str : strs)
        {
            size_t contentSize = str.size();

            // Must be enough to ensure buffer overflows
            for (int i = 0; i < nRepeats; ++i)
            {
                REQUIRE(filebuf.sputn(str.c_str(), contentSize) == contentSize);
            }

            // This would cause an ostream to stop outputting
            // Initially we were also calling underflow, causing us to return eof
            REQUIRE(filebuf.pubsync() != -1);

            // Ensure we wrote the correct amount,
            // Also, tests that external usage of the file does not break the buffer's state
            fseek(f, 0, SEEK_END);
            int totSize = ftell(f);

            static int expectedSize = 0;
            expectedSize += outputSize(str) * nRepeats;

            REQUIRE(totSize == expectedSize);
        }

        std::vector<char> got{};
        got.resize(expected.size());
        filebuf.pubseekpos(0); // moves f's cursor (fseek(f, 0, SEEK_SET);)
        fread(got.data(), sizeof(char), got.size(), f);
        fclose(f);

        REQUIRE(memcmp(expected.data(), got.data(), expected.size()) == 0);
    }

    SECTION("Multiple Output")
    {
        // When implementing the optimized xsputn method,
        // when encountering big blocks we output them instantly
        // (avoids unnecessary transfers to/from buffer)
        // but we forgot to clear the buffer first

        FILE * f = fopen("tempOut.txt", "w+");

        sp::details::OutFileBuf filebuf{f};
        std::string txt = allStrings + allStrings;

        constexpr std::size_t nBlocks = 4;
        std::size_t offsets[nBlocks + 1]{0, 1, 15, 175, txt.size()};
        static_assert(
            175 - 15 > sp::details::OutFileBuf::bufferSize,
            "Must be considered a big block"
        );

        for (std::size_t i = 0; i < nBlocks; ++i)
        {
            std::size_t start     = offsets[i];
            std::size_t blockSize = offsets[i + 1] - start;
            REQUIRE(filebuf.sputn(txt.c_str() + start, blockSize) == blockSize);
        }
        filebuf.pubsync();

        std::vector<char> got{};
        got.resize(txt.size());


        fseek(f, 0, SEEK_SET); // moves f's cursor ()
        fread(got.data(), sizeof(char), txt.size(), f);
        fclose(f);

        REQUIRE(memcmp(txt.data(), got.data(), txt.size()) == 0);
    }


    SECTION("Input")
    {
        FILE * f = fopen("tempIn.txt", "w+");
        fwrite(expected.data(), sizeof(char), expected.size(), f);

        sp::details::InFileBuf filebuf{f};
        for (std::string str : strs)
        {
            size_t sz = str.size();

            auto holder = std::make_unique<char[]>(sz);
            for (int i = 0; i < nRepeats; ++i)
            {
                REQUIRE(filebuf.sgetn(holder.get(), sz) == sz);
                // do not use strings to compare, we have null chars inside.
                REQUIRE(std::memcmp(holder.get(), str.c_str(), sz) == 0);
            }
        }

        fclose(f);
    }


    SECTION("IO")
    {
        FILE * f = fopen("tempIO.txt", "w+");

        sp::details::IOFileBuf filebuf{f};
        for (std::string str : strs)
        {
            size_t sz = str.size();

            // verifies that strings containing null still gives us the expected
            // size printf("size %u\n", sz);

            // Must be enough to ensure buffer overflows
            for (int i = 0; i < nRepeats; ++i)
            {
                REQUIRE(filebuf.sputn(str.c_str(), sz) == sz);
            }
            filebuf.pubsync();

            // Ensure we wrote the correct amount,
            // Also, tests that external usage of the file does not break the buffer's state
            fseek(f, 0, SEEK_END);
            int totSize = ftell(f);

            static int expectedSize = 0;
            expectedSize += outputSize(str) * nRepeats;
            REQUIRE(totSize == expectedSize);

            auto holder = std::make_unique<char[]>(sz);
            for (int i = 0; i < nRepeats; ++i)
            {
                REQUIRE(filebuf.sgetn(holder.get(), sz) == sz);
                // do not use strings to compare, we have null chars inside.
                REQUIRE(std::memcmp(holder.get(), str.c_str(), sz) == 0);
            }
        }

        fclose(f);
    }


    SECTION("Wide chars IO")
    {
        FILE * f = fopen("tempWideIO.txt", "w+");

        std::vector<std::wstring> wstrs{};
        for (const auto & s : strs)
        {
            std::wstring ws{};
            for (char c : s)
            {
                // fails if not unsigned, for the allChars str,
                // => \255 becomes EOF and a character is missed for every iteration
                ws.push_back((wchar_t)(unsigned char)c);
            }
            wstrs.push_back(ws);
        }

        sp::details::wIOFileBuf filebuf{f};
        for (std::wstring str : wstrs)
        {
            size_t sz = str.size();

            // verifies that strings containing null still gives us the expected
            // size
            // printf("size %u\n", sz);

            // Must be enough to ensure buffer overflows
            for (int i = 0; i < nRepeats; ++i)
            {
                REQUIRE(filebuf.sputn(str.c_str(), sz) == sz);
            }
            filebuf.pubsync();

            // Ensure we wrote the correct amount,
            // Also, tests that external usage of the file does not break the buffer's state
            fseek(f, 0, SEEK_END);
            int totSize = ftell(f);

            static int expectedSize = 0;

            // My windows appends \r as single byte, not as wchar
            std::size_t addedChars = outputSize(str) - sz; 
            size_t byteSize         = sz * sizeof(wchar_t);
            expectedSize += (byteSize+addedChars) * nRepeats;

            // REQUIRE(totSize == expectedSize);

            auto holder = std::make_unique<wchar_t[]>(sz);
            for (int i = 0; i < nRepeats; ++i)
            {
                REQUIRE(filebuf.sgetn(holder.get(), sz) == sz);
                // do not use strings to compare, we have null chars inside.
                REQUIRE(std::memcmp(holder.get(), str.c_str(), byteSize) == 0);
            }
        }

        fclose(f);
    }
}
