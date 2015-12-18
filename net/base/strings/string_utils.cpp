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

#include "net/base/strings/string_utils.h"

namespace base {
namespace strings {

bool Equal(const std::string& str1, const std::string& str2, bool bIgnoreCase /*= false*/)
{
    if (str1.length() != str2.length())
        return false;
    auto fnEqual = bIgnoreCase ? std::function<bool(char, char)>(
                                    [](char c1, char c2) -> bool {
                                        return std::tolower(c1, std::locale()) == std::tolower(c2, std::locale());
                                    })
                               : std::function<bool(char, char)>(
                                    [](char c1, char c2) -> bool { return c1 == c2; });
    for (size_t i = 0; i < str1.length(); ++i)
    {
        if (!fnEqual(str1[i], str2[i]))
            return false;
    }
    return true;
}

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

std::string ToLower(const std::string& str)
{
    std::string strLower(str);
    ToLowerSelf(strLower);
    return strLower;
}

std::string ToUpper(const std::string& str)
{
    std::string strUpper(str);
    ToUpperSelf(strUpper);
    return strUpper;
}

void ToLowerSelf(std::string& str)
{
    for (size_t i = 0; i < str.length(); ++i)
    {
        str[i] = std::tolower(str[i], std::locale());
    }
}

void ToUpperSelf(std::string& str)
{
    for (size_t i = 0; i < str.length(); ++i)
    {
        str[i] = std::toupper(str[i], std::locale());
    }
}

void TrimLeftSelf(std::string& str, const std::string& strTrimChars)
{
    if (strTrimChars.empty())
        return;
    while (!str.empty())
    {
        if (std::string::npos != strTrimChars.find_first_of(str.front()))
            str.erase(str.begin());
        else
            return;
    }
}

void TrimRightSelf(std::string& str, const std::string& strTrimChars)
{
    if (strTrimChars.empty())
        return;
    while (!str.empty())
    {
        if (std::string::npos != strTrimChars.find_first_of(str.back()))
            str.pop_back();
        else
            return;
    }
}

void TrimSelf(std::string& str, const std::string& strTrimChars)
{
    TrimLeftSelf(str, strTrimChars);
    TrimRightSelf(str, strTrimChars);
}

std::string TrimLeft(const std::string& str, const std::string& strTrimChars)
{
    std::string strTrimmed(str);
    TrimLeftSelf(strTrimmed, strTrimChars);
    return strTrimmed;
}

std::string TrimRight(const std::string& str, const std::string& strTrimChars)
{
    std::string strTrimmed(str);
    TrimRightSelf(strTrimmed, strTrimChars);
    return strTrimmed;
}

std::string Trim(const std::string& str, const std::string& strTrimChars)
{
    std::string strTrimmed(str);
    TrimLeftSelf(strTrimmed, strTrimChars);
    TrimRightSelf(strTrimmed, strTrimChars);
    return strTrimmed;
}

void TrimLeftSpaceSelf(std::string& str)
{
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4309)
#endif
    std::string strTrimChars = "\t\n\v\f\r ";
    strTrimChars.append({ static_cast<char>(0x85), static_cast<char>(0xA0) });
    TrimLeftSelf(str, strTrimChars);
#ifdef _WIN32
#pragma warning(pop)
#endif
}

void TrimRightSpaceSelf(std::string& str)
{
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4309)
#endif
    std::string strTrimChars = "\t\n\v\f\r ";
    strTrimChars.append({ static_cast<char>(0x85), static_cast<char>(0xA0) });
    TrimRightSelf(str, strTrimChars);
#ifdef _WIN32
#pragma warning(pop)
#endif
}

void TrimSpaceSelf(std::string& str)
{
    TrimLeftSpaceSelf(str);
    TrimRightSpaceSelf(str);
}

std::string TrimLeftSpace(const std::string& str)
{
    std::string strTrimmed(str);
    TrimLeftSpaceSelf(strTrimmed);
    return strTrimmed;
}

std::string TrimRightSpace(const std::string& str)
{
    std::string strTrimmed(str);
    TrimRightSpaceSelf(strTrimmed);
    return strTrimmed;
}

std::string TrimSpace(const std::string& str)
{
    std::string strTrimmed(str);
    TrimLeftSpaceSelf(strTrimmed);
    TrimRightSpaceSelf(strTrimmed);
    return strTrimmed;
}

void TrimPrefixSelf(std::string& str, const std::string& strPrefix,
                    bool bIgnoreCase /*= false*/, bool bRecursive /*= false*/)
{
    if (str.empty() || strPrefix.empty())
        return;
    do 
    {
        if (!StartsWith(str, strPrefix, bIgnoreCase))
            break;
        str.erase(0, strPrefix.length());
    } while (bRecursive);
}

void TrimSuffixSelf(std::string& str, const std::string& strSuffix,
                    bool bIgnoreCase /*= false*/, bool bRecursive /*= false*/)
{
    if (str.empty() || strSuffix.empty())
        return;
    do 
    {
        if (!EndsWith(str, strSuffix, bIgnoreCase))
            break;
        str.erase(str.length() - strSuffix.length(), strSuffix.length());
    } while (bRecursive);
}

std::string TrimPrefix(const std::string& str, const std::string& strPrefix,
                       bool bIgnoreCase /*= false*/, bool bRecursive /*= false*/)
{
    std::string strTrimmed(str);
    TrimPrefixSelf(strTrimmed, strPrefix, bIgnoreCase, bRecursive);
    return strTrimmed;
}

std::string TrimSuffix(const std::string& str, const std::string& strSuffix,
                       bool bIgnoreCase /*= false*/, bool bRecursive /*= false*/)
{
    std::string strTrimmed(str);
    TrimSuffixSelf(strTrimmed, strSuffix, bIgnoreCase, bRecursive);
    return strTrimmed;
}

static std::vector<std::string>
Split(const std::string& str, const std::string& sep,
      bool bAfter, int n, bool bIgnoreCase)
{
    std::vector<std::string> splitList;
    do 
    {
        if (0 == n)
            break;
        if (sep.empty())
        {
            // FIXME: Here we maybe split |str| by its encoding(UTF-8, UTF-16, GBK or others).
            splitList.resize((n < 0 || n > (int)str.length()) ? str.length() : n);
            if (n < 0)
                std::copy(str.begin(), str.end(), splitList.begin());
            else
            {
                int i = 0;
                for (; i < n - 1; ++i)
                {
                    splitList[i] = str.substr(i, 1);
                }
                splitList[i] = str.substr(i);
            }
            break;
        }
        // We define a particular equal function for performance.
        auto fnEqual =
            bIgnoreCase ?
                std::function<bool(const std::string&, size_t, const std::string&)> (
                    [] (const std::string& str, size_t start,
                        const std::string& sep) {
                        for (size_t i = 0; i < sep.length(); ++start, ++i)
                        {
                            if (std::tolower(str[start], std::locale()) != std::tolower(sep[i], std::locale()))
                                return false;
                        }
                        return true;
                    })  : std::function<bool(const std::string&, size_t, const std::string&)>(
                            [] (const std::string& str, size_t start,
                                const std::string& sep) {
                                for (size_t i = 0; i < sep.length(); ++start, ++i)
                                {
                                    if (str[start] != sep[i])
                                        return false;
                                }
                                return true;
                            });
        size_t start = 0;
        size_t maxCount = n < 0 ? SIZE_MAX : n;
        for (size_t i = 0; i + sep.length() <= str.length() && splitList.size() < maxCount - 1;)
        {
            if (fnEqual(str, i, sep))
            { 
                splitList.push_back(str.substr(start, bAfter ? (i - start) + sep.length() : i - start));
                i += sep.length();
                start = i;
                continue;
            }
            ++i;
        }
        splitList.push_back(str.substr(start, str.length() - start));
    } while (0);
    return splitList;
}
std::vector<std::string> SplitN(const std::string& str, const std::string& sep,
                                int n, bool bIgnoreCase /*= false*/)
{
    return Split(str, sep, false, n, bIgnoreCase);
}

std::vector<std::string> SplitAfterN(const std::string& str, const std::string& sep,
                                     int n, bool bIgnoreCase /*= false*/)
{
    return Split(str, sep, true, n, bIgnoreCase);
}

std::vector<std::string> Split(const std::string& str, const std::string& sep,
                               bool bIgnoreCase /*= false*/)
{
    return Split(str, sep, false, -1, bIgnoreCase);
}

std::vector<std::string> SplitAfter(const std::string& str, const std::string& sep,
                                    bool bIgnoreCase /*= false*/)
{
    return Split(str, sep, true, -1, bIgnoreCase);
}

bool IsDigit(const std::string& str, bool bHex /*= false*/)
{
    for (auto v : str)
    {
        if (bHex)
        {
            if (!std::isxdigit(v, std::locale()))
                return false;
        }
        else if (!std::isdigit(v, std::locale()))
            return false;
    }
    return true;
}

} // !namespace strings
} // !namespace base