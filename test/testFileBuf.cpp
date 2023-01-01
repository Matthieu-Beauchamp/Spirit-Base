#include "SPIRIT/Logging/details/FileBuf.hpp"
#include "catch2/catch_all.hpp"

#include <stdio.h>
#include <string>
#include <vector>


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

    std::string allChars{};
    for (int i = 0x00; i < 0xFF + 1; ++i) { allChars.push_back((char)i); }

    std::string nullCharInside{};
    nullCharInside.push_back('a');
    nullCharInside.push_back('\0');
    nullCharInside.push_back('b');


    int nRepeats = 50;
    std::vector<std::string> strs{
        "Hello",
        nullCharInside,
        "",
        "\n",
        "1234567890qwertyuiopasdfghjklzxcvbnm,./"
        ";'[]{}:\"<>?`~!@#$%^&*()_+-=\t",
        allChars};

    // string are unreliable, we have null chars
    std::vector<char> expected{};
    for (const auto & s : strs)
        for (int _ = 0; _ < nRepeats; ++_)
            for (char c : s) expected.push_back(c);

    SECTION("Output")
    {
        FILE * f = fopen("tempOut.txt", "w+");

        sp::details::OutFileBuf filebuf{f};
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

            // This would cause an ostream to stop outputting
            // Initially we were also calling underflow, causing us to return eof
            REQUIRE(filebuf.pubsync() != -1);

            // Ensure we wrote the correct amount,
            // Also, tests that external usage of the file does not break the buffer's state
            fseek(f, 0, SEEK_END);
            int totSize = ftell(f);

            static int expectedSize = 0;
            expectedSize += sz * nRepeats;
            REQUIRE(totSize == expectedSize);
        }

        std::vector<char> got{};
        got.resize(expected.size());
        filebuf.pubseekpos(0); // moves f's cursor (fseek(f, 0, SEEK_SET);)
        fread(got.data(), sizeof(char), got.size(), f);
        fclose(f);

        REQUIRE(memcmp(expected.data(), got.data(), expected.size()) == 0);
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
            expectedSize += sz * nRepeats;
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
            size_t sz       = str.size();
            size_t byteSize = sz * (sizeof(wchar_t) / sizeof(char));

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
            expectedSize += byteSize * nRepeats;
            REQUIRE(totSize == expectedSize);

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
