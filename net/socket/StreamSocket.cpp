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
#include "net/socket/StreamSocket.h"
#include "net/socket/StreamSocketImpl.h"

namespace net {

StreamSocket::StreamSocket()
    : Socket(std::make_shared<StreamSocketImpl>())
{
}


StreamSocket::StreamSocket(const SocketAddress& address)
    : Socket(std::make_shared<StreamSocketImpl>())
{
    Connect(address);
}

StreamSocket::StreamSocket(std::shared_ptr<SocketImpl> pImpl)
    : Socket(pImpl)
{
    assert(std::dynamic_pointer_cast<StreamSocketImpl>(GetImpl()));
}

StreamSocket::StreamSocket(const Socket& socket)
    : Socket(socket)
{
    assert(std::dynamic_pointer_cast<StreamSocketImpl>(GetImpl()));
}

StreamSocket::~StreamSocket()
{
}

bool StreamSocket::Connect(const SocketAddress & address, const std::chrono::seconds & timeout)
{
    return GetImpl()->Connect(address, timeout);
}

bool StreamSocket::ShutdownReceive()
{
    return GetImpl()->ShutdownReceive();
}

bool StreamSocket::ShutdownSend()
{
    return GetImpl()->ShutdownSend();
}

bool StreamSocket::Shutdown()
{
    return GetImpl()->Shutdown();
}

int StreamSocket::Send(const void* buffer, int length, int flags /*= 0*/)
{
    return GetImpl()->Send(reinterpret_cast<const char*>(buffer), length, flags);
}

int StreamSocket::Receive(void * buffer, int length, int flags)
{
    return GetImpl()->Receive(reinterpret_cast<char*>(buffer), length, flags);
}

int StreamSocket::SendUrgent(unsigned char data)
{
    return GetImpl()->SendUrgent(data);
}

} //!net