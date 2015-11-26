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

#include <cassert>

#include "net/socket/SocketAddress.h"

namespace net {

struct SocketAddressPrivate
{
    std::string Host;
    uint16_t Port;
    sockaddr_in Address;
};

SocketAddress::SocketAddress()
{
}

SocketAddress::SocketAddress(const std::string& strHost, uint16_t port)
    : m_data(std::make_shared<SocketAddressPrivate>())
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = strHost.empty() ? htonl(ADDR_ANY) : inet_addr(strHost.c_str());
    addr.sin_port = htons(port);

    m_data->Host = strHost;
    m_data->Port = port;
    m_data->Address = addr;
}

SocketAddress::SocketAddress(const struct sockaddr* addr, int length)
    : m_data(std::make_shared<SocketAddressPrivate>())
{
    assert(sizeof(sockaddr_in) == length);
    memcpy(&m_data->Address, addr, length);
    m_data->Host = inet_ntoa(m_data->Address.sin_addr);
    m_data->Port = ntohs(m_data->Address.sin_port);
}

SocketAddress::SocketAddress(const SocketAddress& address)
    : m_data(address.m_data)
{
}

SocketAddress::~SocketAddress()
{
}

SocketAddress::operator bool() const
{
    return bool(m_data);
}

std::string SocketAddress::GetHost() const
{
    if (m_data)
        return m_data->Host;
    return "";
}

uint16_t SocketAddress::GetPort() const
{
    if (m_data)
        return m_data->Port;
    return 0;
}

const struct sockaddr* SocketAddress::GetAddress() const
{
    if (m_data)
        return reinterpret_cast<const struct sockaddr*>(&m_data->Address);
    return nullptr;
}

int SocketAddress::GetLength() const
{
    if (m_data)
        return sizeof(m_data->Address);
    return 0;
}

} //!net