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

#include "net/base/url.h"

using namespace net::http;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestSuite
{
    TEST_CLASS(Url_Test)
    {
    public:
        TEST_METHOD(Test_Parse)
        {
            std::string strUrl = "http://www.google.com/?q=abc#frag";
            auto url = Url::Parse(strUrl);
            Assert::IsTrue(url.GetScheme() == "http");
            Assert::IsTrue(url.GetHost() == "www.google.com");
            Assert::IsTrue(url.GetPath() == "/");
            Assert::IsTrue(url.GetRawQuery() == "q=abc");
            Assert::IsTrue(url.GetFragment() == "frag");

            strUrl = "http://www.google.com:8080?q=abc#frag";
            url = Url::Parse(strUrl);
            Assert::IsTrue(url.GetScheme() == "http");
            Assert::IsTrue(url.GetHost() == "www.google.com");
            Assert::IsTrue(url.GetPath() == "/");
            Assert::IsTrue(url.GetPort() == 8080);
            Assert::IsTrue(url.GetRawQuery() == "q=abc");
            Assert::IsTrue(url.GetFragment() == "frag");

            strUrl = "file:///usr/home/r.txt";
            url = Url::Parse(strUrl);
            Assert::IsTrue(url.GetScheme() == "file");
            Assert::IsTrue(url.GetHost().empty());
            Assert::IsTrue(url.GetPath() == "/usr/home/r.txt");

            strUrl = ":///usr/home/r.txt";
            url = Url::Parse(strUrl);
            Assert::IsTrue(url.GetScheme().empty());
            Assert::IsTrue(url.GetHost().empty());
            Assert::IsTrue(url.GetPath().empty());

            strUrl = "/usr/home/r.txt";
            url = Url::Parse(strUrl);
            Assert::IsTrue(url.GetScheme().empty());
            Assert::IsTrue(url.GetHost().empty());
            Assert::IsTrue(url.GetPath() == strUrl);

            strUrl = "http:/usr/home/r.txt";
            url = Url::Parse(strUrl);
            Assert::IsTrue(url.GetScheme() == "http");
            Assert::IsTrue(url.GetHost().empty());
            Assert::IsTrue(url.GetPath().empty());

            strUrl = "//www.google.com/search?q=r";
            url = Url::Parse(strUrl);
            Assert::IsTrue(url.GetScheme().empty());
            Assert::IsTrue(url.GetHost() == "www.google.com");
            Assert::IsTrue(url.GetPath() == "/search");
            Assert::IsTrue(url.GetRawQuery() == "q=r");

            strUrl = "mailto:hello@gmail.com";
            url = Url::Parse(strUrl);
            Assert::IsTrue(url.GetScheme() == "mailto");
            Assert::IsTrue(url.GetHost().empty());
            Assert::IsTrue(url.GetPath().empty());
            Assert::IsTrue(url.GetOpaque() == "hello@gmail.com");
        }
    };
}