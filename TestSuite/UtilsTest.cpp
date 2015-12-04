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
#include "net/base/strings/utils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestSuite
{
    TEST_CLASS(Strings_UtilsTest)
    {
    public:
        TEST_METHOD(TestStartsWith)
        {
            std::string strData = "Hello, 世界";
            std::string strSearch = "Hello, ";
            Assert::IsTrue(base::strings::StartsWith(strData, strSearch));
            strSearch = "hello,";
            Assert::IsTrue(base::strings::StartsWith(strData, strSearch, true));
            strSearch = "Helloxxx";
            Assert::IsTrue(base::strings::StartsWith(strData, strSearch) == false);
            strSearch = "";
            Assert::IsTrue(base::strings::StartsWith(strData, strSearch));
            strSearch = strData;
            Assert::IsTrue(base::strings::StartsWith(strData, strSearch));
            strSearch = "HELLO, 世";
            Assert::IsTrue(base::strings::StartsWith(strData, strSearch, true));
        }

        TEST_METHOD(TestEndsWith)
        {
            std::string strData = "Hello, 世界";
            std::string strSearch = " 世界";
            Assert::IsTrue(base::strings::EndsWith(strData, strSearch));
            strSearch = "O, 世界";
            Assert::IsTrue(base::strings::EndsWith(strData, strSearch, true));
            strSearch = "Helloxxx";
            Assert::IsTrue(base::strings::EndsWith(strData, strSearch) == false);
            strSearch = "";
            Assert::IsTrue(base::strings::EndsWith(strData, strSearch));
            strSearch = strData;
            Assert::IsTrue(base::strings::EndsWith(strData, strSearch));
            strSearch = "HELLO, 世界";
            Assert::IsTrue(base::strings::EndsWith(strData, strSearch, true));
        }
    };
}