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

#include "net/http/server.h"

#include <thread>

#include "net/base/strings/string_utils.h"
#include "net/http/connection.h"

namespace net {
namespace http {

namespace {

} // !namespace anonymous

Server::Server(const SocketAddress & address)
    : m_address(address)
{
}

std::shared_ptr<Server> Server::Create(uint16_t port)
{
    return Create(SocketAddress("", port));
}

std::shared_ptr<Server> Server::Create(const std::string & address)
{
    auto hostPort = base::strings::Split(address, ":");
    if (hostPort.size() > 2)
        return nullptr;
    std::string host;
    uint16_t port = 0;
    do 
    {
        if (hostPort.size() == 0)
            break;
        host = hostPort[0];
        if (hostPort.size() == 2)
        {
            try
            {
                port = (uint16_t)std::stoi(hostPort[1]);
            }
            catch (...)
            {
                port = 0;
            }
        }
    } while (0);
    
    return Create(SocketAddress(host, port));
}

std::shared_ptr<Server> Server::Create(const SocketAddress & address)
{
    return std::shared_ptr<Server>(new Server(address));
}

std::chrono::seconds Server::GetReadTimeout() const
{
    return m_readTimeout;
}

void Server::SetReadTimeout(std::chrono::seconds timeout)
{
    m_readTimeout = timeout;
}

std::chrono::seconds Server::GetWriteTimeout() const
{
    return m_writeTimeout;
}

void Server::SetWriteTimeout(std::chrono::seconds timeout)
{
    m_writeTimeout = timeout;
}

std::shared_ptr<Handler> Server::GetHandler() const
{
    return m_handler;
}

void Server::SetHandler(std::shared_ptr<Handler> handler)
{
    m_handler = handler;
}

bool Server::ListenAndServe()
{
    if (!m_ss.Bind(m_address))
        return false;
    if (!m_ss.Listen())
        return false;

    while (true)
    {
        auto s = m_ss.Accept();

        s->SetReceiveTimeout(m_readTimeout);
        s->SetSendTimeout(m_writeTimeout);

        std::thread t(&Server::Serve, this, s);
        t.detach();
    }
    return true;
}

void Server::Serve(std::shared_ptr<StreamSocket> s)
{
    Connection conn(s);

    while (true)
    {
        auto request = conn.ReadRequest();
        if (!request)
            return;
        if (m_handler)
        {
            auto ctx = Context::Create(s, request);
            m_handler->ServeHTTP(ctx);
        }
        auto c = request->GetHeader("Connection");
        if (base::strings::Equal(c, "close", true))
            break;
    }
}

} // !namespace http
} // !namespace net