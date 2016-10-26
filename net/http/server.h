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

#include <string>

#include "net/http/handler.h"
#include "net/socket/ServerSocket.h"
#include "net/socket/SocketAddress.h"

namespace net {
namespace http {

class Server
{
public:
    ~Server() {}

protected:
    Server(const SocketAddress& address);

public:
    static std::shared_ptr<Server> Create(uint16_t port);
    static std::shared_ptr<Server> Create(const std::string& address);
    static std::shared_ptr<Server> Create(const SocketAddress& address);

    std::chrono::seconds GetReadTimeout() const;
    void SetReadTimeout(std::chrono::seconds timeout);
    std::chrono::seconds GetWriteTimeout() const;
    void SetWriteTimeout(std::chrono::seconds timeout);

    std::shared_ptr<Handler> GetHandler() const;
    void SetHandler(std::shared_ptr<Handler> handler);

    bool ListenAndServe();

protected:
    void Serve(std::shared_ptr<StreamSocket> s);

private:
    SocketAddress m_address;
    ServerSocket m_ss;
    std::chrono::seconds m_readTimeout;
    std::chrono::seconds m_writeTimeout;
    std::shared_ptr<Handler> m_handler;
};

} // !namespace http
} // !namespace net