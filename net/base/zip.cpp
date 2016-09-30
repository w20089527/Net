// The MIT License (MIT)
//
// Copyright(c) 2015 huan.wang
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files(the "Software"),
// to deal in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software,
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <cassert>
#include <iomanip>
#include <memory>
#include <sstream>

#include "net/base/zip.h"
#include "third_party/zlib/zlib.h"

namespace base {
namespace zip {

    size_t GetCompressLength(size_t srcLength)
    {
        return static_cast<size_t>(compressBound(static_cast<uLong>(srcLength)));
    }

    size_t Compress(char* dst, size_t dstLength, const char* src, size_t srcLength)
    {
        assert(dst && dstLength && src && srcLength);
        uLongf outlength = (uLongf)dstLength;
        if (Z_OK != compress(reinterpret_cast<Bytef*>(dst), &outlength,
            reinterpret_cast<const Bytef*>(src), (uLong)srcLength))
            return 0;
        return static_cast<size_t>(outlength);
    }

    std::string Compress(const char* data, size_t length)
    {
        assert(data && length);
        size_t dstlen = GetCompressLength(length);
        assert(dstlen);
        try
        {
            std::unique_ptr<char[]> dst(new char[dstlen + 8]);
            if (!dst)
                return "";

            std::ostringstream oss;
            oss << std::setw(8) << std::setfill('0') << length;
            std::memcpy(dst.get(), oss.str().c_str(), 8);

            size_t actualSize = Compress(dst.get() + 8, dstlen, data, length);
            return std::string(dst.get(), 8 + actualSize);
        }
        catch (...)
        {
            return "";
        }
    }

    std::string Compress(const std::string& data)
    {
        return Compress(data.c_str(), data.length());
    }

    size_t Decompress(char* dst, size_t dstlen, const char* src, size_t srclen)
    {
        assert(dst && dstlen && src && srclen >= 8);
        uLongf outlen = (uLongf)dstlen;
        if (Z_OK != uncompress(reinterpret_cast<Bytef*>(dst), &outlen,
            reinterpret_cast<const Bytef*>(src), (uLong)srclen))
            return 0;
        return static_cast<size_t>(outlen);
    }

    std::string Decompress(const char* data, size_t length)
    {
        if (length < 8)
            return "";
        try
        {
            auto size = std::stoul(std::string(data, 8));
            if (size == 0)
                return "";
            std::unique_ptr<char[]> dst(new char[size]);
            if (!dst)
                return "";
            size_t actualSize = Decompress(dst.get(), size, data + 8, length - 8);
            return std::string(dst.get(), actualSize);
        }
        catch (...)
        {
            return "";
        }
    }

    std::string Decompress(const std::string& data)
    {
        return Decompress(data.c_str(), data.length());
    }

    size_t GDecompress(char* dst, size_t dstlen, const char* src, size_t srclen)
    {
        z_stream stream;
        int err;

        stream.next_in = (z_const Bytef *)src;
        stream.avail_in = (uInt)srclen;
        /* Check for source > 64K on 16-bit machine: */
        if ((uLong)stream.avail_in != srclen) return 0;

        stream.next_out = (z_const Bytef *)dst;
        stream.avail_out = dstlen;

        stream.zalloc = (alloc_func)0;
        stream.zfree = (free_func)0;

        err = inflateInit2(&stream, 16 + MAX_WBITS);
        if (err != Z_OK) return 0;

        err = inflate(&stream, Z_FINISH);
        if (err != Z_STREAM_END) {
            inflateEnd(&stream);
            if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
                return 0;
            return 0;
        }
        auto actualSize = stream.total_out;

        inflateEnd(&stream);
        return actualSize;
    }

    std::string GDecompress(const char * data, size_t length)
    {
        if (length < 4)
            return "";
        try
        {
            unsigned int b[4];
            b[0] = (unsigned int)data[length - 4] & 0x000000ff;
            b[1] = (unsigned int)data[length - 3] & 0x000000ff;
            b[2] = (unsigned int)data[length - 2] & 0x000000ff;
            b[3] = (unsigned int)data[length - 1] & 0x000000ff;
            auto size = b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
            if (size == 0)
                return "";
            std::unique_ptr<char[]> dst(new char[size]);
            if (!dst)
                return "";
            size_t actualSize = GDecompress(dst.get(), size, data, length);
            return std::string(dst.get(), actualSize);
        }
        catch (...)
        {
            return "";
        }
    }

    std::string GDecompress(const std::string& data)
    {
        return GDecompress(data.c_str(), data.length());
    }

} // !zip
} // !base