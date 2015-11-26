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
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "stdafx.h"
#include "EchoServer.h"

#pragma comment(lib, "Ws2_32.lib")

EchoServer::EchoServer()
    : m_serverSocket(net::SocketAddress("", 0))
{
    m_thread = std::thread([&]() {
        std::chrono::seconds timeout(1);
        while (!m_bStop)
        {
            if (!m_serverSocket.Poll(timeout, net::SELECT_READ))
                continue;
            auto ss = m_serverSocket.Accept();
            char buffer[256] = { 0 };
            int n = ss->Receive(buffer, sizeof(buffer));
            while (n > 0 && !m_bStop)
            {
                ss->Send(buffer, n);
                n = ss->Receive(buffer, sizeof(buffer));
            }
        } //!while
    });
    std::this_thread::sleep_for(std::chrono::seconds(2));
}


EchoServer::~EchoServer()
{
    m_bStop = true;
    m_thread.join();
}

uint16_t EchoServer::GetPort() const
{
    return m_serverSocket.GetLocalAddress().GetPort();
}
