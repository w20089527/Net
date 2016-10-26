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

#include "net/http/connection.h"

#include "net/base/escape.h"
#include "net/http/utils.h"

namespace net {
namespace http {

Connection::Connection(std::shared_ptr<StreamSocket> s)
    : m_streamSocket(s)
{
    m_reader.Reset(s.get());
}

std::shared_ptr<Request> Connection::ReadRequest()
{
    auto startLine = m_reader.ExtractStartLine();
    auto spList = base::strings::SplitN(startLine, " ", 3);
    if (spList.size() != 3)
        return nullptr;
    
    bool error;
    auto headers = m_reader.ExtractHeaders(error);
    if (error || headers.size() == 0)
        return nullptr;
    
    Header h;
    ParseHeader(headers, h);
    
    // Parse Url.
    auto hostIter = h.find("host");
    if (h.end() == hostIter)
        return nullptr;

    // Currently, we just support http scheme.
    auto request = Request::Create(spList[0], "http://" + hostIter->second + spList[1]);
    if (!request)
        return nullptr;
    request->SetHeader(h);

    Values formValues;
    auto query = request->GetUrl().GetRawQuery();
    ParseQueryForm(query, formValues);
    request->SetFormValues(formValues);

    m_reader.ExtractRequestMessage(request);

    auto remoteAddress = m_streamSocket->GetForeignAddress();
    request->SetRemoteAddress(remoteAddress.GetHost() + std::to_string(remoteAddress.GetPort()));

    return request;
}

} // !namespace http
} // !namespace net