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

#include "net/base/escape.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestSuite
{
    TEST_CLASS(escape_Test)
    {
    public:
        TEST_METHOD(Test_Escape)
        {
            std::string str = u8"Hello, %20世界";
            Assert::IsTrue(base::Escape(str, std::set<char>{','}) == "Hello,%20%2520%E4%B8%96%E7%95%8C");
            Assert::IsTrue(base::Escape(str, std::set<char>{','}, false, true) == "Hello,+%2520%E4%B8%96%E7%95%8C");
            Assert::IsTrue(base::Escape(str, std::set<char>{','}, true, true) == "Hello,+%20%E4%B8%96%E7%95%8C");
            Assert::IsTrue(base::Escape(str) == "Hello%2C%20%2520%E4%B8%96%E7%95%8C");
            Assert::IsTrue(base::Escape(str, true) == "Hello%2C%20%20%E4%B8%96%E7%95%8C");

            std::string url = u8"http://www.google.com/search?q=世 界";
            Assert::IsTrue(base::EscapeUrl(url) == "http://www.google.com/search?q=%E4%B8%96%20%E7%95%8C");
            Assert::IsTrue(base::EscapeUrl(url, true) == "http://www.google.com/search?q=%E4%B8%96+%E7%95%8C");
            Assert::IsTrue(base::EscapeUrlComponent(url) == "http%3A%2F%2Fwww.google.com%2Fsearch%3Fq%3D%E4%B8%96%20%E7%95%8C");
            Assert::IsTrue(base::EscapeUrlComponent(url, true) == "http%3A%2F%2Fwww.google.com%2Fsearch%3Fq%3D%E4%B8%96+%E7%95%8C");
            Assert::IsTrue(base::EscapeUrlEncodedData(url) == "http://www.google.com/search%3Fq%3D%E4%B8%96%20%E7%95%8C");
            Assert::IsTrue(base::EscapeUrlEncodedData(url, true) == "http://www.google.com/search%3Fq%3D%E4%B8%96+%E7%95%8C");
        }

        TEST_METHOD(Test_Unescape)
        {
            std::string str = "Hello,+%20%2520%E4%B8%96%E7%95%8C%AW";
            Assert::IsTrue(base::Unescape(str) == u8"Hello,+ %20世界%AW");
            Assert::IsTrue(base::Unescape(str, true) == u8"Hello,  %20世界%AW");

            std::string url = u8"http://www.google.com/search?q=世 界";
            Assert::IsTrue(base::Unescape("http%3A%2F%2Fwww.google.com%2Fsearch%3Fq%3D%E4%B8%96%20%E7%95%8C") == url);
            Assert::IsTrue(base::Unescape("http%3A%2F%2Fwww.google.com%2Fsearch%3Fq%3D%E4%B8%96+%E7%95%8C", true) == url);
        }

        TEST_METHOD(Test_EscapeForHTML_UnescapeForHTML)
        {
            std::string str = "<html>\"Hello&'world'\"</html>";
            Assert::IsTrue(base::EscapeForHTML(str) == "&lt;html&gt;&quot;Hello&amp;&#39;world&#39;&quot;&lt;/html&gt;");
            Assert::IsTrue(base::UnescapeForHTML("&lt;html&gt;&quot;Hello&amp;&#39;world&#39;&quot;&lt;/html&gt;") == str);
            Assert::IsTrue(base::UnescapeForHTML("&LT;html&gt;&QUOT;Hello&amp;&#39;world&#39;&quot;&lt;/html&gt;") == str);
        }
    };
}