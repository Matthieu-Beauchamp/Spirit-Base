////////////////////////////////////////////////////////////
//
// Spirit
// Copyright (C) 2022 Matthieu Beauchamp-Boulay
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////


#ifndef SPIRIT_FILEBUF_HPP
#define SPIRIT_FILEBUF_HPP

#include "SPIRIT/Base/Configuration/config.hpp"
#include <array>
#include <stdio.h>
#include <streambuf>

namespace sp
{
namespace details
{

template <typename char_type>
class FileBufBase : public std::basic_streambuf<char_type>
{
public:

    typedef typename std::basic_streambuf<char_type>::traits_type traits_type;
    typedef typename std::basic_streambuf<char_type>::int_type int_type;
    typedef typename std::basic_streambuf<char_type>::pos_type pos_type;
    typedef typename std::basic_streambuf<char_type>::off_type off_type;


    FileBufBase(FILE * file) : targetFile{file} { this->seekpos(0); }

    ~FileBufBase() override = default;

    // Current position in the file is undefined.
    FILE *
    file() const
    {
        return targetFile;
    }

protected:

    ////////////////////////////////////////////////////////////
    // Locales
    ////////////////////////////////////////////////////////////

    // handled by std::basic_streambuf, we don't need it


    ////////////////////////////////////////////////////////////
    // Positioning
    ////////////////////////////////////////////////////////////

    // should be private...?
    std::size_t tell() const
    {return ftell(targetFile) / sizeof(char_type);}

    // TODO: Changing the buffer is not yet supported (should it?)

    // offset is assumed to be in terms of char_type.
    // for example if using char_type = wchar,
    //      then seekoff(1, beg) will move the cursor two bytes from the
    //      beginning of the FILE.
    pos_type
    seekoff(
        off_type offset,
        std::ios_base::seekdir basePos,
        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out
    ) override
    {
        // values of enumeration are implementation defined
        int Cpos = SEEK_CUR;
        switch (basePos)
        {
            case std::ios_base::beg: Cpos = SEEK_SET; break;
            case std::ios_base::cur: Cpos = SEEK_CUR; break;
            case std::ios_base::end: Cpos = SEEK_END; break;
        }
        fseek(targetFile, static_cast<sp::Int32>(offset * sizeof(char_type)), Cpos);

        return tell();
    }

    pos_type
    seekpos(
        pos_type pos,
        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out
    ) override
    {
        return seekoff(pos, std::ios_base::beg, which);
    }

    int
    sync() override
    {
        return this->overflow(traits_type::eof());
    }

    ////////////////////////////////////////////////////////////
    // shortcuts
    ////////////////////////////////////////////////////////////

    static bool
    isEof(int_type ch)
    {
        return traits_type::eq_int_type(ch, traits_type::eof());
    }

    static bool
    isNotEof(int_type ch)
    {
        return !isEof(ch);
    }

    ////////////////////////////////////////////////////////////
    // read and write behave in accordance with char_type.
    //  seekpos is the position of the file cursor in terms of char_type.
    //  n is the number of char_type to read
    ////////////////////////////////////////////////////////////

    // TODO: Windows appends a char '\r' when we are working with wchar_t

    // read, diff
    std::pair<size_t, size_t>
    read(char_type * dest, pos_type seekPos, std::size_t n)
    {
        seekpos(seekPos, std::ios_base::in);

#if defined(SPIRIT_OS_WINDOWS)
        // Windows may skip \r when reading \n, which will not be included in the
        // value returned by fread. We need to manually check the position diff
        std::size_t start = tell();
        std::size_t nRead = fread(dest, sizeof(char_type), n, targetFile);
        std::size_t diff  = tell() - start;
#else
        std::size_t nRead    = fread(dest, sizeof(char_type), n, targetFile);
        std::size_t diff     = nRead;
#endif

        return {nRead, diff};
    }

    // {wrote, filePtrDiff}
    std::pair<std::size_t, std::size_t>
    write(const char_type * src, pos_type seekPos, std::size_t n)
    {
        seekpos(seekPos, std::ios_base::out);
#if defined(SPIRIT_OS_WINDOWS)
        // Windows may append \r after \n, which will not be included in the
        // value returned by fwrite. We need to manually check the position diff
        std::size_t start            = tell();
        std::size_t nWritten = fwrite(src, sizeof(char_type), n, targetFile);
        std::size_t diff     = tell() - start;
#else
        std::size_t nWritten = fwrite(src, sizeof(char_type), n, targetFile);
        std::size_t diff     = nWritten;
#endif

        return {nWritten, diff};
    }

    ////////////////////////////////////////////////////////////
    // Errors
    ////////////////////////////////////////////////////////////

    // error is returned and cleared
    int_type
    checkError()
    {
        int_type err = ferror(targetFile);
        clearerr(targetFile);
        return err;
    }

private:

    ////////////////////////////////////////////////////////////
    // Data
    ////////////////////////////////////////////////////////////

    FILE * targetFile;
};


// TODO: Check for optimal size / strategy from other implementations

template <std::streamsize bufSize, typename char_type>
struct ioBuffer
{
private:

    struct array : public std::array<char_type, bufSize>
    {
        typedef std::array<char_type, bufSize> Base;
        // typedef iterator types and stuff...

        using Base::Base;

        // MSVC won't convert std::array's begin and end to T*
        char_type *
        begin()
        {
            return this->data();
        }
        const char_type *
        begin() const
        {
            return this->data();
        }

        char_type *
        end()
        {
            return this->data() + this->size();
        }
        const char_type *
        end() const
        {
            return this->data() + this->size();
        }
    };

public:

    array buf;
    std::streamsize pos = 0;
};


// Allows disabling output or input buffers depending on mode
// Note that the in/out buffer members must always exists,
//  otherwise we must define separe specializations for FileBuf.
// The unused buffers will instead have size 0
template <std::ios_base::openmode mode, std::streamsize bufSize, typename char_type>
struct ioBuffers
{
    void
    assignPos(std::streamsize pos, std::ios_base::openmode which)
    {
    }
};

template <std::streamsize bufSize, typename char_type>
struct ioBuffers<std::ios_base::in, bufSize, char_type>
{
    void
    assignPos(std::streamsize pos, std::ios_base::openmode which)
    {
        if (which & std::ios_base::in)
            in.pos = pos;
    }

    sp::details::ioBuffer<bufSize, char_type> in;
    sp::details::ioBuffer<0, char_type> out;
};

template <std::streamsize bufSize, typename char_type>
struct ioBuffers<std::ios_base::out, bufSize, char_type>
{
    void
    assignPos(std::streamsize pos, std::ios_base::openmode which)
    {
        if (which & std::ios_base::out)
            out.pos = pos;
    }

    sp::details::ioBuffer<0, char_type> in;
    sp::details::ioBuffer<bufSize, char_type> out;
};

template <std::streamsize bufSize, typename char_type>
struct ioBuffers<std::ios_base::in | std::ios_base::out, bufSize, char_type>
{
    void
    assignPos(std::streamsize pos, std::ios_base::openmode which)
    {
        if (which & std::ios_base::in)
            in.pos = pos;
        if (which & std::ios_base::out)
            out.pos = pos;
    }

    sp::details::ioBuffer<bufSize, char_type> in;
    sp::details::ioBuffer<bufSize, char_type> out;
};


////////////////////////////////////////////////////////////
/// \brief Stream buffer for working with C FILE* types
///
/// This class does not open or close files.
///
/// The main reason for this class is to use stdout for colored
/// output. To know whether stdout is connected to a terminal or
/// an actual file, we need the FILE* since we cannot test this
/// on ostreams (ie cout)
///
////////////////////////////////////////////////////////////
template <std::ios_base::openmode mode, std::streamsize bufSize, typename char_type>
class SPIRIT_API FileBuf : public sp::details::FileBufBase<char_type>
{
    typedef sp::details::FileBufBase<char_type> Base;

public:

    typedef
        typename sp::details::FileBufBase<char_type>::traits_type traits_type;
    typedef typename sp::details::FileBufBase<char_type>::int_type int_type;
    typedef typename sp::details::FileBufBase<char_type>::pos_type pos_type;
    typedef typename sp::details::FileBufBase<char_type>::off_type off_type;

    static constexpr std::streamsize bufferSize = bufSize;

    static_assert(bufSize > 0, "Requires at least a single char slot in buffer");


    FileBuf(FILE * file) : Base{file}
    {
        // 0 is the only guaranteed safe position (in case of wide chars)
        this->seekpos(0);

        this->setp(io.out.buf.begin(), io.out.buf.end());
        this->setg(io.in.buf.begin(), io.in.buf.end(), io.in.buf.end());
    }

    ~FileBuf() override = default;

protected:

    ////////////////////////////////////////////////////////////
    // Locales
    ////////////////////////////////////////////////////////////

    // handled by std::basic_streambuf, we don't need it


    ////////////////////////////////////////////////////////////
    // Positioning
    ////////////////////////////////////////////////////////////

    // TODO: Changing the buffer is not yet supported

    pos_type
    seekoff(
        off_type offset,
        std::ios_base::seekdir basePos,
        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out
    ) override
    {
        pos_type newPos = Base::seekoff(offset, basePos, which);
        io.assignPos(newPos, which);
        return newPos;
    }

    // TODO: methods for telling input and output cursor positions

    ////////////////////////////////////////////////////////////
    // Get Area
    ////////////////////////////////////////////////////////////

    // TODO: xgetn can be overloaded for better efficiency


    std::streamsize
    showmanyc() override
    {
        if (mode & std::ios_base::in)
        {
            pos_type current = io.in.pos;
            pos_type last
                = this->seekoff(0, std::ios_base::end, std::ios_base::in);
            this->seekpos(current, std::ios_base::in);

            return (last - current);
        }

        return -1;
    }


    int_type
    underflow() override
    {
        if (mode & std::ios_base::in)
        {
            if (this->gptr() < this->egptr())
                return traits_type::to_int_type(*this->gptr());

            std::streamsize available = showmanyc();
            if (available == -1)
                return traits_type::eof();

            auto nRead = Base::read(io.in.buf.begin(), io.in.pos, bufSize);
            io.in.pos += nRead.second;

            this->setg(
                io.in.buf.begin(),
                io.in.buf.begin(),
                io.in.buf.begin() + nRead.first
            );

            if (Base::checkError() != 0)
                return traits_type::eof();

            return nRead.first == 0 ? traits_type::eof()
                              : traits_type::to_int_type(*this->gptr());
        }

        return traits_type::eof();
    }

    ////////////////////////////////////////////////////////////
    // Put Area
    ////////////////////////////////////////////////////////////

    std::streamsize
    xsputn(const char_type * str, std::streamsize n) override
    {
        if (n > bufSize)
        {
            this->overflow(traits_type::eof());
            auto wrote
                = Base::write(str, io.out.pos, static_cast<std::size_t>(n));
            io.out.pos += wrote.second;
            return wrote.first;
        }
        else
        {
            return Base::xsputn(str, n);
        }
    }

    int_type
    overflow(int_type ch) override
    {
        if (mode & std::ios_base::out)
        {
            size_t filled = this->pptr() - this->pbase();
            auto wrote    = Base::write(io.out.buf.begin(), io.out.pos, filled);
            io.out.pos += wrote.second;

            this->setp(
                io.out.buf.begin(),
                io.out.buf.end()
            ); // resets current pointer

            if (Base::isNotEof(ch))
            {
                char_type excess   = traits_type::to_char_type(ch);
                io.out.buf.front() = excess;
                this->pbump(1);
            }

            if (Base::checkError() != 0)
                return traits_type::eof();

            return 0; // unspecified but eof is -1
        }

        return traits_type::eof();
    }

    ////////////////////////////////////////////////////////////
    // Putback
    ////////////////////////////////////////////////////////////

    // TODO: pbackfail

private:

    sp::details::ioBuffers<mode, bufSize, char_type> io{};
};

constexpr static std::streamsize bufSizeDefault = 128;
typedef char charTypeDefault;
typedef wchar_t wCharTypeDefault;

typedef SPIRIT_API
    sp::details::FileBuf<std::ios_base::out, bufSizeDefault, charTypeDefault>
        OutFileBuf;

typedef SPIRIT_API
    sp::details::FileBuf<std::ios_base::in, bufSizeDefault, charTypeDefault>
        InFileBuf;

typedef SPIRIT_API sp::details::
    FileBuf<std::ios_base::in | std::ios_base::out, bufSizeDefault, charTypeDefault>
        IOFileBuf;

typedef SPIRIT_API
    sp::details::FileBuf<std::ios_base::out, bufSizeDefault, wCharTypeDefault>
        wOutFileBuf;

typedef SPIRIT_API
    sp::details::FileBuf<std::ios_base::in, bufSizeDefault, wCharTypeDefault>
        wInFileBuf;

typedef SPIRIT_API sp::details::
    FileBuf<std::ios_base::in | std::ios_base::out, bufSizeDefault, wCharTypeDefault>
        wIOFileBuf;


} // namespace details
} // namespace sp


#endif // SPIRIT_FILEBUF_HPP
