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

#include <vector>

#include "net/http/common.h"

namespace net {
namespace http {

std::string CommonRequestResponse::GetProto() const
{
    return "HTTP/" + std::to_string(m_protoMajor) + "." + std::to_string(m_protoMinor);
}

void CommonRequestResponse::SetProto(int protoMajor, int protoMinor)
{
    m_protoMajor = protoMajor;
    m_protoMinor = protoMinor;
}

bool CommonRequestResponse::GetClose() const
{
    return m_close;
}

void CommonRequestResponse::SetClose(bool close)
{
    m_close = close;
}

std::string CommonRequestResponse::GetHeader(const std::string & key) const
{
    auto v = m_header.find(key);
    if (v == m_header.end())
    {
        return "";
    }
    return v->second;
}

std::vector<std::string> CommonRequestResponse::GetHeaders(const std::string & key) const
{
    std::vector<std::string> valueList;
    auto vlist = m_header.equal_range(key);
    for (auto iter = vlist.first; iter != vlist.second; ++iter)
    {
        valueList.push_back(iter->second);
    }
    return valueList;
}

Header CommonRequestResponse::GetHeaders() const
{
    return m_header;
}

void CommonRequestResponse::SetHeader(const Header & header)
{
    m_header = header;
}

void CommonRequestResponse::SetHeader(const std::string & key, const std::string & value)
{
    auto validKey = base::strings::TrimSpace(key);
    if (validKey.empty())
    {
        return;
    }
    auto v = m_header.find(validKey);
    if (v != m_header.end())
    {
        v->second = value;
    }
    else
    {
        m_header.emplace(validKey, value);
    }
}

} // !namespace http
} // !namespace net