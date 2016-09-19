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

namespace net {
namespace http {

std::shared_ptr<Context> Context::NewContext(std::shared_ptr<StreamSocket> connection, std::shared_ptr<Request> request)
{
    return std::shared_ptr<Context>(new Context(connection, request));
}

std::shared_ptr<Request> Context::GetRequest() const
{
    return m_request;
}

int Context::Write(const void * buffer, int length)
{
    if (m_connection)
    {
        return m_connection->Send(buffer, length);
    }
    return -1;
}

int Context::Write(const std::string & buffer)
{
    return Write((const void*)buffer.c_str(), buffer.length());
}

} // !namespace http
} // !namespace net