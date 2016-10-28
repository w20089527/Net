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

#include "net/http/context.h"

#include "net/base/zip.h"

namespace net {
namespace http {

Context::Context(std::shared_ptr<StreamSocket> connection, std::shared_ptr<Request> request)
    : m_connection(connection)
{
    m_response = Response::Create();
    m_response->SetRequest(request);
}

std::shared_ptr<Context> Context::Create(std::shared_ptr<StreamSocket> connection, std::shared_ptr<Request> request)
{
    return std::shared_ptr<Context>(new Context(connection, request));
}

std::shared_ptr<Request> Context::GetRequest() const
{
    return m_response->GetRequest();
}

std::shared_ptr<Response> Context::GetResponse() const
{
    return m_response;
}

int Context::WriteHeader()
{
    if (!m_connection)
        return -1;

    std::string message = m_response->GetProto() + " ";
    message += std::to_string(m_response->GetStatusCode()) + " ";
    message += m_response->GetStatus() + "\r\n";

    auto headers = m_response->GetHeaders();
    for (auto iter = headers.begin(); iter != headers.end(); ++iter)
    {
        message += iter->first + ": " + iter->second + "\r\n";
    }
    message += "\r\n";
    int len = m_connection->Send(message.c_str(), message.length());
    if (len == (int)message.length())
        m_wroteHeader = true;
    return len;
}

int Context::Write(const void * buffer, int length)
{
    return Write(std::string((const char*)buffer, length));
}

int Context::Write(const std::string & buffer)
{
    if (!m_connection)
        return -1;

    std::string message;
    if (!m_wroteHeader)
    {
        message = m_response->GetProto() + " ";
        message += std::to_string(m_response->GetStatusCode()) + " ";
        message += m_response->GetStatus() + "\r\n";

        m_response->SetHeader("Content-Length", std::to_string(buffer.length()));
        auto headers = m_response->GetHeaders();
        for (auto iter = headers.begin(); iter != headers.end(); ++iter)
        {
            message += iter->first + ": " + iter->second + "\r\n";
        }
        message += "\r\n";
    }
    message += buffer;
    return m_connection->Send(message.c_str(), message.length());
}

} // !namespace http
} // !namespace net