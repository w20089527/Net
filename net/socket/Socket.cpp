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

#include "net/socket/Socket.h"

namespace net {
Socket::Socket()
    : m_pImpl(new SocketImpl())
{
}

Socket::Socket(std::shared_ptr<SocketImpl> pImpl)
    : m_pImpl(pImpl)
{
}

Socket::Socket(const Socket & socket)
    : m_pImpl(socket.m_pImpl)
{
}

Socket::~Socket()
{
}

bool Socket::operator==(const Socket& socket) const
{
    return m_pImpl == socket.m_pImpl;
}

bool Socket::operator!=(const Socket & socket) const
{
    return m_pImpl != socket.m_pImpl;
}

bool Socket::operator<(const Socket & socket) const
{
    return m_pImpl < socket.m_pImpl;
}

Socket& Socket::operator=(const Socket& socket)
{
    if (&socket != this)
        m_pImpl = socket.m_pImpl;
    return *this;
}

bool Socket::operator<=(const Socket& socket) const
{
    return m_pImpl <= socket.m_pImpl;
}

bool Socket::operator>(const Socket & socket) const
{
    return m_pImpl > socket.m_pImpl;
}

bool Socket::operator>=(const Socket & socket) const
{
    return m_pImpl >= socket.m_pImpl;
}

int Socket::GetAvailableBytes() const
{
    assert(m_pImpl);
    return m_pImpl->GetAvailableBytes();
}

SocketAddress Socket::GetLocalAddress() const
{
    assert(m_pImpl);
    return m_pImpl->GetLocalAddress();
}

SocketAddress Socket::GetForeignAddress() const
{
    assert(m_pImpl);
    return m_pImpl->GetForeignAddress();
}

bool Socket::Poll(const std::chrono::microseconds& timeout, int mode)
{
    assert(m_pImpl);
    return m_pImpl->Poll(timeout, mode);
}

bool Socket::Close()
{
    assert(m_pImpl);
    return m_pImpl->Close();
}

bool Socket::SetRawOption(int level, int option, const void * value, int length)
{
    assert(m_pImpl);
    return m_pImpl->SetRawOption(level, option, value, length);
}

bool Socket::SetOption(int level, int option, int value)
{
    assert(m_pImpl);
    return m_pImpl->SetOption(level, option, value);
}

bool Socket::SetOption(int level, int option, unsigned value)
{
    assert(m_pImpl);
    return m_pImpl->SetOption(level, option, value);
}

bool Socket::SetOption(int level, int option, unsigned char value)
{
    assert(m_pImpl);
    return m_pImpl->SetOption(level, option, value);
}

bool Socket::SetOption(int level, int option, const std::chrono::seconds & value)
{
    assert(m_pImpl);
    return m_pImpl->SetOption(level, option, value);
}

bool Socket::GetRawOption(int level, int option, void * value, int & length) const
{
    assert(m_pImpl);
    return m_pImpl->GetRawOption(level, option, value, length);
}

bool Socket::GetOption(int level, int option, int & value) const
{
    assert(m_pImpl);
    return m_pImpl->GetOption(level, option, value);
}

bool Socket::GetOption(int level, int option, unsigned & value) const
{
    assert(m_pImpl);
    return m_pImpl->GetOption(level, option, value);
}

bool Socket::GetOption(int level, int option, unsigned char & value) const
{
    assert(m_pImpl);
    return m_pImpl->GetOption(level, option, value);
}

bool Socket::GetOption(int level, int option, std::chrono::seconds & value) const
{
    assert(m_pImpl);
    return m_pImpl->GetOption(level, option, value);
}

bool Socket::SetSendBufferSize(int size)
{
    assert(m_pImpl);
    return m_pImpl->SetSendBufferSize(size);
}

int Socket::GetSendBufferSize() const
{
    assert(m_pImpl);
    return m_pImpl->GetSendBufferSize();
}

bool Socket::SetReceiveBufferSize(int size)
{
    assert(m_pImpl);
    return m_pImpl->SetReceiveBufferSize(size);
}

int Socket::GetReceiveBufferSize() const
{
    assert(m_pImpl);
    return m_pImpl->GetReceiveBufferSize();
}

bool Socket::SetSendTimeout(const std::chrono::seconds & timeout)
{
    assert(m_pImpl);
    return m_pImpl->SetSendTimeout(timeout);
}

std::chrono::seconds Socket::GetSendTimeout() const
{
    assert(m_pImpl);
    return m_pImpl->GetSendTimeout();
}

bool Socket::SetReceiveTimeout(const std::chrono::seconds & timeout)
{
    assert(m_pImpl);
    return m_pImpl->SetReceiveTimeout(timeout);
}

std::chrono::seconds Socket::GetReceiveTimeout() const
{
    assert(m_pImpl);
    return m_pImpl->GetReceiveTimeout();
}

bool Socket::SetLinger(bool on, int seconds)
{
    assert(m_pImpl);
    return m_pImpl->SetLinger(on, seconds);
}

bool Socket::GetLinger(bool & on, int & seconds) const
{
    assert(m_pImpl);
    return m_pImpl->GetLinger(on, seconds);
}

bool Socket::SetNoDelay(bool flag)
{
    assert(m_pImpl);
    return m_pImpl->SetNoDelay(flag);
}

bool Socket::GetNoDelay() const
{
    assert(m_pImpl);
    return m_pImpl->GetNoDelay();
}

bool Socket::SetKeepAlive(bool flag)
{
    assert(m_pImpl);
    return m_pImpl->SetKeepAlive(flag);
}

bool Socket::GetKeepAlive() const
{
    assert(m_pImpl);
    return m_pImpl->GetKeepAlive();
}

bool Socket::SetReuseAddress(bool flag)
{
    assert(m_pImpl);
    return m_pImpl->SetReuseAddress(flag);
}

bool Socket::GetReuseAddress() const
{
    assert(m_pImpl);
    return m_pImpl->GetReuseAddress();
}

bool Socket::SetOOBInline(bool flag)
{
    assert(m_pImpl);
    return m_pImpl->SetOOBInline(flag);
}

bool Socket::GetOOBInline() const
{
    assert(m_pImpl);
    return m_pImpl->GetOOBInline();
}

bool Socket::SetBlocking(bool flag)
{
    assert(m_pImpl);
    return m_pImpl->SetBlocking(flag);
}

bool Socket::GetBlocking() const
{
    assert(m_pImpl);
    return m_pImpl->GetBlocking();
}

NativeHandle Socket::GetNativeHandle() const
{
    assert(m_pImpl);
    return m_pImpl->GetNativeHandle();
}

std::shared_ptr<SocketImpl> Socket::GetImpl() const
{
    return m_pImpl;
}

bool Socket::Init(int af)
{
    assert(m_pImpl);
    return m_pImpl->Init(af);
}

} //!net