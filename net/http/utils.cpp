// The MIT License (MIT)
//
// Copyright(c) 2016 huan.wang
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

#include "net/http/utils.h"

#include "net/base/escape.h"

namespace net {
namespace http {

void ParseQueryForm(const std::string & query, Values & formValues)
{
    auto kvpairs = base::strings::Split(query, "&");
    for (auto kv : kvpairs)
    {
        auto pair = base::strings::SplitN(kv, "=", 2);
        auto k = base::strings::TrimSpace(pair[0]);
        if (k.empty())
            continue;
        formValues.emplace(k, pair.size() == 2 ? base::Unescape(pair[1]) : "");
    }
}

Values ParseQueryForm(const std::string & query)
{
    Values formValues;
    ParseQueryForm(query, formValues);
    return formValues;
}

void ParseHeader(const std::vector<std::string>& rawHeaderList, Header & header)
{
    for (auto h : rawHeaderList)
    {
        auto kv = base::strings::SplitN(h, ":", 2);
        auto k = base::strings::TrimSpace(kv[0]);
        if (k.empty())
            continue;
        header.emplace(k, kv.size() == 2 ? base::strings::TrimSpace(kv[1]) : "");
    }
}

Header ParseHeader(const std::vector<std::string>& rawHeaderList)
{
    Header h;
    ParseHeader(rawHeaderList, h);
    return h;
}

} // !namespace http
} // !namespace net