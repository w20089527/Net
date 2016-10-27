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

#pragma once

#include "net/http/request.h"
#include "net/http/response.h"
#include "net/socket/StreamSocket.h"

namespace net {
namespace http {

class Context
{
public:
    ~Context() {}

private:
    Context(
        std::shared_ptr<StreamSocket> connection,
        std::shared_ptr<Request> request);

public:
    static std::shared_ptr<Context>
        Create(
            std::shared_ptr<StreamSocket> connection,
            std::shared_ptr<Request> request);

    std::shared_ptr<Request> GetRequest() const;
    std::shared_ptr<Response> GetResponse() const;

    int Write(const void* buffer, int length);
    int Write(const std::string& buffer);

private:
    std::shared_ptr<StreamSocket> m_connection;
    std::shared_ptr<Response> m_response;
};

} // !namespace http
} // !namespace net
