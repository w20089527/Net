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

#include "net/socket/DatagramSocket.h"
#include "net/socket/DatagramSocketImpl.h"

namespace net {

DatagramSocket::DatagramSocket()
    : Socket(std::make_shared<DatagramSocketImpl>())
{
}


DatagramSocket::DatagramSocket(const SocketAddress& address, bool bReuse /*= false*/)
    : Socket(std::make_shared<DatagramSocketImpl>())
{
    Bind(address, bReuse);
}

DatagramSocket::DatagramSocket(const Socket& socket)
    : Socket(socket)
{
    assert(std::dynamic_pointer_cast<DatagramSocketImpl>(GetImpl()));
}

DatagramSocket::DatagramSocket(std::shared_ptr<SocketImpl> pImpl)
    : Socket(pImpl)
{
    assert(std::dynamic_pointer_cast<DatagramSocketImpl>(GetImpl()));
}

DatagramSocket::~DatagramSocket()
{
}

DatagramSocket& DatagramSocket::operator=(const Socket& socket)
{
    if (std::dynamic_pointer_cast<DatagramSocketImpl>(socket.GetImpl()))
        Socket::operator=(socket);
    else
    {
        assert(0);
    }
    return *this;
}

bool DatagramSocket::Connect(const SocketAddress& address)
{
    return GetImpl()->Connect(address);
}

bool DatagramSocket::Bind(const SocketAddress& address, bool bReuse /*= false*/)
{
    return GetImpl()->Bind(address, bReuse);
}

int DatagramSocket::Send(const void* buffer, int length, int flags /*= 0*/)
{
    return GetImpl()->Send(reinterpret_cast<const char*>(buffer), length, flags);
}

int DatagramSocket::Receive(void* buffer, int length, int flags /*= 0*/)
{
    return GetImpl()->Receive(reinterpret_cast<char*>(buffer), length, flags);
}

int DatagramSocket::SendTo(const void* buffer, int length, const SocketAddress& address, int flags /*= 0*/)
{
    return GetImpl()->SendTo(reinterpret_cast<const char*>(buffer), length, address, flags);
}

int DatagramSocket::ReceiveFrom(void* buffer, int length, SocketAddress& address, int flags /*= 0*/)
{
    return GetImpl()->ReceiveFrom(reinterpret_cast<char*>(buffer), length, address, flags);
}

bool DatagramSocket::SetBroadcast(bool flag)
{
    return GetImpl()->SetBroadcast(flag);
}

bool DatagramSocket::GetBroadcast()
{
    return GetImpl()->GetBroadcast();
}

} //!net