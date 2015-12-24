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

#include <locale>
#include <map>

#include "net/base/escape.h"
#include "net/base/strings/string_utils.h"

namespace base {

namespace {
    
    const std::string kHexString = "0123456789ABCDEF";
    

} // !namespace

std::string Escape(const std::string& str, const std::set<char>& excludedSet,
                   bool bKeepEscaped /*= false*/, bool bUsePlus /*= false*/)
{
    std::string strResult;
    strResult.reserve(str.length() * 3);
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (bUsePlus && ' ' == str[i])
            strResult.push_back('+');
        else if (bKeepEscaped && '%' == str[i]
            && (i + 2) < str.length()
            && base::strings::IsDigit(str[i + 1], true)
            && base::strings::IsDigit(str[i + 2], true))
            strResult.push_back('%');
        else if (excludedSet.find(str[i]) != excludedSet.end()
            || std::isalnum(str[i], std::locale()))
            strResult.push_back(str[i]);
        else
        {
            auto c = static_cast<unsigned char>(str[i]);
            strResult.push_back('%');
            strResult.push_back(kHexString[c >> 4]);
            strResult.push_back(kHexString[c & 0x0f]);
        }
    }
    return strResult;
}

std::string Escape(const std::string& str, bool bKeepEscaped /*= false*/)
{
    return Escape(str, { '*', '+', '-', '.', '/', '@' }, bKeepEscaped, false);
}

std::string EscapeUrl(const std::string& strUrl, bool bUsePlus /*= false*/)
{
    return Escape(strUrl, { '!','#','$','&','\'','(',')','*','+',',',
                            '-','.','/',':',';','=','?','@','~' },
                  false, bUsePlus);
}

std::string EscapeUrlComponent(const std::string& strUrl, bool bUsePlus /*= false*/)
{
    return Escape(strUrl, { '!','\'','(',')','*','-','.','~' }, false, bUsePlus);
}

std::string EscapeUrlEncodedData(const std::string& strData, bool bUsePlus /*= false*/)
{
    return Escape(strData, { '(',')','*',',','-','.','/',':','@','_','~' }, false, bUsePlus);
}

std::string Unescape(const std::string& str, bool bReplacePlus /*= false*/)
{
    std::string strResult;
    strResult.reserve(str.size());
    for (size_t i = 0; i < str.length();)
    {
        if (bReplacePlus && '+' == str[i])
            strResult.push_back(' ');
        else if ('%' == str[i]
                && i + 2 < str.length()
                && base::strings::IsDigit(str[i + 1], true)
                && base::strings::IsDigit(str[i + 2], true))
        {
            char c = (base::strings::HexDigitToInt(str[i + 1]) << 4)
                        + base::strings::HexDigitToInt(str[i + 2]);
            strResult.push_back(c);
            i += 3;
            continue;
        }
        else
            strResult.push_back(str[i]);
        ++i;
    }
    return strResult;
}

std::string EscapeForHTML(const std::string& str)
{
    typedef std::map<char, std::string> EscapeMap;
    static const EscapeMap kHtmlEscapeMap {
        { '<', "&lt;" },
        { '>', "&gt;" },
        { '&', "&amp;" },
        { '"', "&quot;" },
        { '\'', "&#39;" },
    };

    std::string strResult;
    strResult.reserve(str.capacity());
    for (auto v : str)
    {
        auto i = kHtmlEscapeMap.find(v);
        if (i == kHtmlEscapeMap.end())
        {
            strResult.push_back(v);
            continue;
        }
        strResult.append(i->second);
    }
    return strResult;
}

std::string UnescapeForHTML(const std::string& str)
{
    typedef std::map<std::string, char> EscapeMap;
    static const EscapeMap kHtmlEscapeMap{
        { "&lt;", '<' },
        { "&gt;", '>' },
        { "&amp;", '&' },
        { "&quot;", '"' },
        { "&#39;", '\'' },
    };

    std::string strResult;
    strResult.reserve(str.length());
    for (size_t i = 0; i < str.size();)
    {
        if ('&' != str[i])
        {
            strResult.push_back(str[i]);
            ++i;
            continue;
        }
        bool bMatched = false;
        for (auto v : kHtmlEscapeMap)
        {
            if (!base::strings::Equal(str.substr(i, v.first.length()), v.first, true))
                continue;
            strResult.push_back(v.second);
            bMatched = true;
            i += v.first.length();
            break;
        }
        if (bMatched)
            continue;
        strResult.push_back(str[i]);
        ++i;
    }
    return strResult;
}

} // !namespace base