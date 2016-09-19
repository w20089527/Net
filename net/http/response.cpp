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

#include "net/http/response.h"

namespace net {
namespace http {

std::shared_ptr<Response> Response::NewResponse()
{
    return std::shared_ptr<Response>(new Response());
}

int Response::GetStatus() const
{
    return m_status;
}

void Response::SetStatus(int code)
{
    m_status = code;
}

std::string Response::GetBody() const
{
    return m_body;
}

void Response::SetBody(const std::string & body)
{
    m_body = body;
}

std::shared_ptr<Request> Response::GetRequest() const
{
    return m_request;
}

void Response::SetRequest(std::shared_ptr<Request> request)
{
    m_request = request;
}

} // !namespace http
} // !namespace net