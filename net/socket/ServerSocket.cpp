// The MIT License (MIT)
//
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

#include "net/socket/ServerSocket.h"
#include "net/socket/ServerSocketImpl.h"

namespace net {

ServerSocket::ServerSocket()
    : Socket(std::make_shared<ServerSocketImpl>())
{
}

ServerSocket::ServerSocket(const Socket& socket)
    : Socket(socket)
{
    assert(std::dynamic_pointer_cast<ServerSocketImpl>(GetImpl()));
}

ServerSocket::ServerSocket(const SocketAddress& address, int backlog /*= 64*/)
    : Socket(std::make_shared<ServerSocketImpl>())
{
    Bind(address, true);
    Listen(backlog);
}

ServerSocket::ServerSocket(uint16_t port, int backlog /*= 64*/)
    : Socket(std::make_shared<ServerSocketImpl>())
{
    Bind(port, true);
    Listen(backlog);
}

ServerSocket::~ServerSocket()
{
}

ServerSocket& ServerSocket::operator=(const Socket& socket)
{
    if (std::dynamic_pointer_cast<ServerSocketImpl>(socket.GetImpl()))
        Socket::operator=(socket);
    else
    {
        assert(0);
    }
    return *this;
}

bool ServerSocket::Bind(const SocketAddress& address, bool bReuse /*= false*/)
{
    return GetImpl()->Bind(address, bReuse);
}

bool ServerSocket::Bind(uint16_t port, bool bReuse /*= false*/)
{
    return Bind(SocketAddress("", port), bReuse);
}

bool ServerSocket::Listen(int backlog /*= 64*/)
{
    return GetImpl()->Listen(backlog);
}

std::shared_ptr<StreamSocket> ServerSocket::Accept()
{
    return std::make_shared<StreamSocket>(GetImpl()->Accept());
}

} //!net