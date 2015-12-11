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
#include "EchoServer.h"
#include "net/socket/SocketAddress.h"
#include "net/socket/StreamSocket.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestSuite
{
    TEST_CLASS(Socket_Test)
    {
    public:

        TEST_METHOD(Test_Echo)
        {
            EchoServer server;
            net::StreamSocket ss;
            ss.Connect(net::SocketAddress("127.0.0.1", server.GetPort()));
            int n = ss.Send("Hello", 5);
            Assert::AreEqual(5, n, L"Send unexpected.");
            char buffer[256] = { 0 };
            n = ss.Receive(buffer, sizeof(buffer));
            Assert::AreEqual(5, n, L"Receive unexpected.");
            Assert::AreEqual("Hello", buffer, L"Incorrect message.");
        }

        TEST_METHOD(Test_Poll)
        {
            EchoServer server;
            net::StreamSocket ss;
            ss.Connect(net::SocketAddress("127.0.0.1", server.GetPort()));
            auto start = std::chrono::system_clock::now();
            Assert::IsTrue(!ss.Poll(std::chrono::seconds(2), net::SELECT_READ));
            auto elapse = std::chrono::duration_cast < std::chrono::seconds > (std::chrono::system_clock::now() - start);
            Assert::IsTrue(elapse >= std::chrono::seconds(1));
            start = std::chrono::system_clock::now();
            Assert::IsTrue(ss.Poll(std::chrono::seconds(1), net::SELECT_WRITE));
            elapse = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start);
            Assert::IsTrue(elapse <= std::chrono::seconds(1));
            ss.Send("Hello", 5);
        }

        TEST_METHOD(Test_Available)
        {
            EchoServer server;
            net::StreamSocket ss;
            ss.Connect(net::SocketAddress("127.0.0.1", server.GetPort()));
            ss.Send("Hello", 5);
            Assert::IsTrue(ss.Poll(std::chrono::seconds(2), net::SELECT_READ));
            int av = ss.GetAvailableBytes();
            Assert::IsTrue(av > 0 && av <= 5);
            char buffer[256] = { 0 };
            int n = ss.Receive(buffer, sizeof(buffer));
            Assert::IsTrue(5 == n);
            Assert::AreEqual("Hello", buffer);
        }
    };
}