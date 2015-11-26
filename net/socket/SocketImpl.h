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

#pragma once

#include <chrono>
#include <memory>
#include <cstdint>
#include <string>
#include <utility>
#include <WinSock2.h>

#include "net/socket/SocketDefs.h"

namespace net {

class SocketImpl
{
public:
    SocketImpl();
    SocketImpl(NativeHandle sockfd);
    virtual ~SocketImpl();
    virtual std::shared_ptr<SocketImpl> Accept();
    virtual bool Bind(const SocketAddress& address, bool bReuse = false);
    virtual bool Connect(const SocketAddress& address, const std::chrono::seconds& timeout = std::chrono::seconds(0));
    virtual bool Listen(int backlog);
    virtual int Receive(char* buffer, int length, int flags = 0);
    virtual int Send(const char* buffer, int length, int flags = 0);
    virtual int ReceiveFrom(char* buffer, int length, SocketAddress& address, int flags = 0);
    virtual int SendTo(const char* buffer, int length, const SocketAddress& address, int flags = 0);
    virtual int SendUrgent(unsigned char data);
    virtual int GetAvailableBytes();
    virtual SocketAddress GetLocalAddress() const;
    virtual SocketAddress GetForeignAddress() const;
    virtual bool Poll(const std::chrono::microseconds& timeout, int mode);
    virtual bool Close();
    virtual bool Shutdown();
    virtual bool ShutdownReceive();
    virtual bool ShutdownSend();
    virtual bool SetRawOption(int level, int option, const void* value, int length);
    bool SetOption(int level, int option, int value);
    bool SetOption(int level, int option, unsigned value);
    bool SetOption(int level, int option, unsigned char value);
    bool SetOption(int level, int option, const std::chrono::seconds& value);
    virtual bool GetRawOption(int level, int option, void* value, int& length) const;
    bool GetOption(int level, int option, int& value) const;
    bool GetOption(int level, int option, unsigned& value) const;
    bool GetOption(int level, int option, unsigned char& value) const;
    bool GetOption(int level, int option, std::chrono::seconds& value) const;
    bool SetSendBufferSize(int size);
    int GetSendBufferSize() const;
    bool SetReceiveBufferSize(int size);
    int GetReceiveBufferSize() const;
    bool SetSendTimeout(const std::chrono::seconds& timeout);
    std::chrono::seconds GetSendTimeout() const;
    bool SetReceiveTimeout(const std::chrono::seconds& timeout);
    std::chrono::seconds GetReceiveTimeout() const;
    bool SetLinger(bool on, int seconds);
    bool GetLinger(bool& on, int& seconds) const;
    bool SetNoDelay(bool flag);
    bool GetNoDelay() const;
    bool SetKeepAlive(bool flag);
    bool GetKeepAlive() const;
    bool SetReuseAddress(bool flag);
    bool GetReuseAddress() const;
    bool SetOOBInline(bool flag);
    bool GetOOBInline() const;
    bool SetBroadcast(bool flag);
    bool GetBroadcast() const;
    virtual bool SetBlocking(bool flag);
    virtual bool GetBlocking() const;
    bool Ioctl(int request, void* arg);
    int GetSocketError() const;
    NativeHandle GetNativeHandle() const;

protected:
    virtual bool Init(int af);
    bool InitSocket(int af, int type = SOCK_STREAM, int proto = 0);
    void Reset(SOCKET sockfd = INVALID_SOCKET);
    
protected:
    NativeHandle m_sockfd = INVALID_SOCKET;
    bool m_bBlocking = true;

private:
    static bool s_bInitialized;

    friend class Socket;
};

} //!net