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

#include "stdafx.h"
#include "CppUnitTest.h"
#include "UDPEchoServer.h"
#include "net/socket/DatagramSocket.h"
#include "net/socket/SocketAddress.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestSuite
{
    TEST_CLASS(DatagramSocketTest)
    {
    public:

        TEST_METHOD(TestUdpEcho)
        {
            UDPEchoServer server;
            net::DatagramSocket ds;
            Assert::IsTrue(ds.Connect(net::SocketAddress("127.0.0.1", server.GetPort())));
            Assert::IsTrue(ds.Send("hello", 5) == 5);
            char buffer[256] = { 0 };
            Assert::IsTrue(ds.Receive(buffer, sizeof(buffer)) == 5);
            Assert::AreEqual("hello", buffer);
        }

        TEST_METHOD(TestSendToReceiveTo)
        {
            UDPEchoServer server;
            net::DatagramSocket ds;
            Assert::IsTrue(ds.SendTo("hello", 5, net::SocketAddress("127.0.0.1", server.GetPort())) == 5);
            char buffer[256] = { 0 };
            net::SocketAddress sa;
            Assert::IsTrue(ds.ReceiveFrom(buffer, sizeof(buffer), sa) == 5);
            Assert::IsTrue(sa.GetPort() == server.GetPort());
            Assert::AreEqual("hello", buffer);
        }

        TEST_METHOD(TestBroadcast)
        {
            UDPEchoServer server;
            net::DatagramSocket ds;
            net::SocketAddress sa("255.255.255.255", server.GetPort());
            ds.SetBroadcast(true);
            Assert::IsTrue(ds.SendTo("hello", 5, sa) == 5);
            char buffer[256] = { 0 };
            Assert::IsTrue(ds.Receive(buffer, sizeof(buffer)) == 5);
            Assert::AreEqual("hello", buffer);
        }
    };
} //!TestSuite