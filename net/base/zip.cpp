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

} // !zip
} // !base