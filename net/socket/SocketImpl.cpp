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

#include "net/socket/SocketImpl.h"
#include "net/socket/StreamSocketImpl.h"

namespace net {

bool SocketImpl::s_bInitialized = false;

SocketImpl::SocketImpl()
{
    if (!s_bInitialized)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR)
        {
            s_bInitialized = true;
            std::atexit([] {
                WSACleanup();
            });
        }
    }
}

SocketImpl::SocketImpl(NativeHandle sockfd)
    : m_sockfd(sockfd)
{
}

SocketImpl::~SocketImpl()
{
    Close();
}

std::shared_ptr<SocketImpl> SocketImpl::Accept()
{
    assert(m_sockfd != INVALID_SOCKET);
    auto clientSocket = accept(m_sockfd, nullptr, nullptr);
    if (INVALID_SOCKET != clientSocket)
        return std::make_shared<StreamSocketImpl>(clientSocket);
    return nullptr;
}

bool SocketImpl::Bind(const SocketAddress& address, bool bReuse /*= false*/)
{
    if (INVALID_SOCKET == m_sockfd)
        Init(AF_INET);
    if (bReuse)
        SetReuseAddress(true);
    return bind(m_sockfd, address.GetAddress(), address.GetLength()) != SOCKET_ERROR;
}

bool SocketImpl::Connect(const SocketAddress& address, const std::chrono::seconds& timeout /*= std::chrono::seconds(0)*/)
{
    if (INVALID_SOCKET == m_sockfd)
        Init(AF_INET);
    if (timeout.count() > 0)
        SetBlocking(false);
    bool bResult = false;
    do 
    {
        if (SOCKET_ERROR == connect(m_sockfd, address.GetAddress(), address.GetLength()))
        {
            if (timeout.count() == 0)
                break;
            int err = WSAGetLastError();
            if (WSAEINPROGRESS != err && WSAEWOULDBLOCK != err)
                break;
            if (!Poll(timeout, SELECT_READ | SELECT_WRITE | SELECT_ERROR))
                break;
            if (GetSocketError() != 0)
                break;
        }
        bResult = true;
    } while (0);
    if (timeout.count() > 0)
        SetBlocking(true);
    return bResult;
}

bool SocketImpl::Listen(int backlog)
{
    assert(INVALID_SOCKET != m_sockfd);
    return listen(m_sockfd, backlog) != SOCKET_ERROR;
}

int SocketImpl::Receive(char* buffer, int length, int flags /*= 0*/)
{
    assert(INVALID_SOCKET != m_sockfd);
    return recv(m_sockfd, buffer, length, flags);
}

int SocketImpl::Send(const char* buffer, int length, int flags /*= 0*/)
{
    assert(INVALID_SOCKET != m_sockfd);
    return send(m_sockfd, buffer, length, flags);
}

int SocketImpl::ReceiveFrom(char* buffer, int length, SocketAddress& address, int flags /*= 0*/)
{
    assert(INVALID_SOCKET != m_sockfd);
    sockaddr_in addr;
    int addrLen = sizeof(addr);
    int rc = recvfrom(m_sockfd, buffer, length, flags, (sockaddr*)&addr, &addrLen);
    if (rc >= 0)
        address = SocketAddress(reinterpret_cast<const struct sockaddr*>(&addr), addrLen);
    return rc;
}

int SocketImpl::SendTo(const char* buffer, int length, const SocketAddress& address, int flags /*= 0*/)
{
    assert(INVALID_SOCKET != m_sockfd);
    return sendto(m_sockfd, buffer, length, flags, address.GetAddress(), address.GetLength());
}

int SocketImpl::SendUrgent(unsigned char data)
{
    assert(INVALID_SOCKET != m_sockfd);
    return send(m_sockfd, reinterpret_cast<const char*>(&data), sizeof(data), MSG_OOB);
}

int SocketImpl::GetAvailableBytes()
{
    int value;
    Ioctl(FIONREAD, &value);
    return value;
}

SocketAddress SocketImpl::GetLocalAddress() const
{
    assert(INVALID_SOCKET != m_sockfd);
    sockaddr_in addr;
    int len = sizeof(addr);
    if (getsockname(m_sockfd, (sockaddr*)&addr, &len) == 0)
        return SocketAddress(reinterpret_cast<const struct sockaddr*>(&addr), len);
    return SocketAddress();
}

SocketAddress SocketImpl::GetForeignAddress() const
{
    assert(INVALID_SOCKET != m_sockfd);
    sockaddr_in addr;
    int len = sizeof(addr);
    if (getpeername(m_sockfd, (sockaddr*)&addr, &len) == 0)
        return SocketAddress(reinterpret_cast<const struct sockaddr*>(&addr), len);
    return SocketAddress();
}

bool SocketImpl::Poll(const std::chrono::microseconds& timeout, int mode)
{
    assert(INVALID_SOCKET != m_sockfd);
    
    fd_set fdRead;
    fd_set fdWrite;
    fd_set fdExcept;
    FD_ZERO(&fdRead);
    FD_ZERO(&fdWrite);
    FD_ZERO(&fdExcept);

    if (mode & SELECT_READ)
    {
        FD_SET(m_sockfd, &fdRead);
    }
    if (mode & SELECT_WRITE)
    {
        FD_SET(m_sockfd, &fdWrite);
    }
    if (mode & SELECT_ERROR)
    {
        FD_SET(m_sockfd, &fdExcept);
    }

    struct timeval tv;
    tv.tv_sec = (long)std::chrono::duration_cast<std::chrono::seconds>(timeout).count();
    tv.tv_usec = (long)(timeout.count() % 1000000);

    return select(int(m_sockfd) + 1, &fdRead, &fdWrite, &fdExcept, &tv) > 0;
}

bool SocketImpl::Close()
{
    if (INVALID_SOCKET != m_sockfd)
    {
        if (closesocket(m_sockfd) == SOCKET_ERROR)
            return false;
        m_sockfd = INVALID_SOCKET;
    }
    return true;
}

bool SocketImpl::Shutdown()
{
    assert(INVALID_SOCKET != m_sockfd);
    return shutdown(m_sockfd, SD_BOTH) != SOCKET_ERROR;
}

bool SocketImpl::ShutdownReceive()
{
    assert(INVALID_SOCKET != m_sockfd);
    return shutdown(m_sockfd, SD_RECEIVE) != SOCKET_ERROR;
}

bool SocketImpl::ShutdownSend()
{
    assert(INVALID_SOCKET != m_sockfd);
    return shutdown(m_sockfd, SD_SEND) != SOCKET_ERROR;
}

bool SocketImpl::SetRawOption(int level, int option, const void* value, int length)
{
    return SOCKET_ERROR != setsockopt(m_sockfd, level, option, reinterpret_cast<const char*>(value), length);
}

bool SocketImpl::SetOption(int level, int option, int value)
{
    return SetRawOption(level, option, &value, sizeof(value));
}

bool SocketImpl::SetOption(int level, int option, unsigned value)
{
    return SetRawOption(level, option, &value, sizeof(value));
}

bool SocketImpl::SetOption(int level, int option, unsigned char value)
{
    return SetRawOption(level, option, &value, sizeof(value));
}

bool SocketImpl::SetOption(int level, int option, const std::chrono::seconds& value)
{
    struct timeval tv;
    tv.tv_sec = (long)value.count();
    tv.tv_usec = 0L;
    return SetRawOption(level, option, &tv, sizeof(tv));
}

bool SocketImpl::GetRawOption(int level, int option, void* value, int& length) const
{
    assert(INVALID_SOCKET != m_sockfd);
    return getsockopt(m_sockfd, level, option, reinterpret_cast<char*>(value), &length) != SOCKET_ERROR;
}

bool SocketImpl::GetOption(int level, int option, int& value) const
{
    int length = sizeof(value);
    return GetRawOption(level, option, &value, length);
}

bool SocketImpl::GetOption(int level, int option, unsigned & value) const
{
    int length = sizeof(value);
    return GetRawOption(level, option, &value, length);
}

bool SocketImpl::GetOption(int level, int option, unsigned char & value) const
{
    int length = sizeof(value);
    return GetRawOption(level, option, &value, length);
}

bool SocketImpl::GetOption(int level, int option, std::chrono::seconds& value) const
{
    struct timeval tv;
    int length = sizeof(tv);
    if (!GetRawOption(level, option, &tv, length))
        return false;
    value = std::chrono::seconds(tv.tv_sec);
    return true;
}

bool SocketImpl::SetSendBufferSize(int size)
{
    return SetOption(SOL_SOCKET, SO_SNDBUF, size);
}

int SocketImpl::GetSendBufferSize() const
{
    int size = 0;
    GetOption(SOL_SOCKET, SO_SNDBUF, size);
    return size;
}

bool SocketImpl::SetReceiveBufferSize(int size)
{
    return SetOption(SOL_SOCKET, SO_RCVBUF, size);
}

int SocketImpl::GetReceiveBufferSize() const
{
    int size = 0;
    GetOption(SOL_SOCKET, SO_RCVBUF, size);
    return size;
}

bool SocketImpl::SetSendTimeout(const std::chrono::seconds& timeout)
{
    int value = (int)std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
    return SetOption(SOL_SOCKET, SO_SNDTIMEO, value);
}

std::chrono::seconds SocketImpl::GetSendTimeout() const
{
    int value = 0;
    GetOption(SOL_SOCKET, SO_SNDTIMEO, value);
    return std::chrono::seconds(value / 1000);
}

bool SocketImpl::SetReceiveTimeout(const std::chrono::seconds& timeout)
{
    int value = (int)std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
    return SetOption(SOL_SOCKET, SO_RCVTIMEO, value);
}

std::chrono::seconds SocketImpl::GetReceiveTimeout() const
{
    int value = 0;
    GetOption(SOL_SOCKET, SO_RCVTIMEO, value);
    return std::chrono::seconds(value / 1000);
}

bool SocketImpl::SetLinger(bool on, int seconds)
{
    struct linger l;
    l.l_onoff = (u_short)on;
    l.l_linger = (u_short)seconds;
    return SetRawOption(SOL_SOCKET, SO_LINGER, &l, sizeof(l));
}

bool SocketImpl::GetLinger(bool& on, int& seconds) const
{
    struct linger l;
    int length = sizeof(l);
    if (!GetRawOption(SOL_SOCKET, SO_LINGER, &l, length))
        return false;
    on = l.l_onoff != 0;
    seconds = l.l_linger;
    return true;
}

bool SocketImpl::SetNoDelay(bool flag)
{
    int value = flag ? 1 : 0;
    return SetOption(IPPROTO_TCP, TCP_NODELAY, value);
}

bool SocketImpl::GetNoDelay() const
{
    int value = 0;
    GetOption(IPPROTO_TCP, TCP_NODELAY, value);
    return value != 0;
}

bool SocketImpl::SetKeepAlive(bool flag)
{
    int value = flag ? 1 : 0;
    return SetOption(SOL_SOCKET, SO_KEEPALIVE, value);
}

bool SocketImpl::GetKeepAlive() const
{
    int value = 0;
    GetOption(SOL_SOCKET, SO_KEEPALIVE, value);
    return value != 0;
}

bool SocketImpl::SetReuseAddress(bool flag)
{
    int value = flag ? 1 : 0;
    return SetOption(SOL_SOCKET, SO_REUSEADDR, value);
}

bool SocketImpl::GetReuseAddress() const
{
    int value = 0;
    GetOption(SOL_SOCKET, SO_REUSEADDR, value);
    return value != 0;
}

bool SocketImpl::SetOOBInline(bool flag)
{
    int value = flag ? 1 : 0;
    return SetOption(SOL_SOCKET, SO_OOBINLINE, value);
}

bool SocketImpl::GetOOBInline() const
{
    int value = 0;
    GetOption(SOL_SOCKET, SO_OOBINLINE, value);
    return value != 0;
}

bool SocketImpl::SetBroadcast(bool flag)
{
    int value = flag ? 1 : 0;
    return SetOption(SOL_SOCKET, SO_BROADCAST, value);
}

bool SocketImpl::GetBroadcast() const
{
    int value = 0;
    GetOption(SOL_SOCKET, SO_BROADCAST, value);
    return value != 0;
}

bool SocketImpl::SetBlocking(bool flag)
{
    int arg = flag ? 1 : 0;
    if (!Ioctl(FIONBIO, &arg))
        return false;
    m_bBlocking = flag;
    return true;
}

bool SocketImpl::GetBlocking() const
{
    return m_bBlocking;
}

bool SocketImpl::Ioctl(int request, void * arg)
{
    assert(INVALID_SOCKET != m_sockfd);
    return ioctlsocket(m_sockfd, request, reinterpret_cast<u_long*>(arg)) != SOCKET_ERROR;
}

int SocketImpl::GetSocketError() const
{
    int value = 0;
    GetOption(SOL_SOCKET, SO_ERROR, value);
    return value;
}

NativeHandle SocketImpl::GetNativeHandle() const
{
    return m_sockfd;
}

bool SocketImpl::Init(int af)
{
    return InitSocket(af);
}

bool SocketImpl::InitSocket(int af, int type /*= SOCK_STREAM*/, int proto /*= 0*/)
{
    assert(INVALID_SOCKET == m_sockfd);
    m_sockfd = socket(af, type, proto);
    return INVALID_SOCKET != m_sockfd;
}

void SocketImpl::Reset(SOCKET sockfd /*= INVALID_SOCKET*/)
{
    m_sockfd = sockfd;
}

} //!net