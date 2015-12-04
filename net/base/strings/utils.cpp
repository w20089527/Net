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
// 

#include <functional>
#include <locale>

#include "net/base/strings/utils.h"

namespace base {
namespace strings {

    template<typename InputIterator1, typename InputIterator2>
    static bool StartsWithOrEndsWith(InputIterator1 First1, InputIterator1 Last1,
                                     InputIterator2 First2, InputIterator2 Last2, bool IgnoreCase)
    {
        if (First2 == Last2)
            return true;
        if (std::distance(First1, Last1) < std::distance(First2, Last2))
            return false;
        auto fnEqual = IgnoreCase ? std::function<bool(InputIterator1, InputIterator1)>(
                                        [](InputIterator1 c1, InputIterator1 c2) -> bool {
                                            return std::tolower(*c1, std::locale()) == std::tolower(*c2, std::locale()); })
                                  : std::function<bool(InputIterator1, InputIterator1)>(
                                        [](InputIterator1 c1, InputIterator1 c2) -> bool { return *c1 == *c2; });
        for (; First1 != Last1 && First2 != Last2; ++First1, ++First2)
        {
            if (!fnEqual(First1, First2))
                return false;
        }
        return true;
    }
    bool StartsWith(const std::string& strData, const std::string& strSearch, bool bIgnoreCase /* = false */)
    {
        return StartsWithOrEndsWith(strData.cbegin(), strData.cend(), strSearch.cbegin(), strSearch.cend(), bIgnoreCase);
    }

    bool EndsWith(const std::string& strData, const std::string& strSearch, bool bIgnoreCase /* = false */)
    {
        return StartsWithOrEndsWith(strData.crbegin(), strData.crend(), strSearch.crbegin(), strSearch.crend(), bIgnoreCase);
    }

} // !namespace strings
} // !namespace base