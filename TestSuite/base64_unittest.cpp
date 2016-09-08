// The MIT License (MIT)
//
// Copyright(c) 2016 huan.wang
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

#include "net/base/base64.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestSuite
{
    TEST_CLASS(base64_Test)
    {
    public:
        TEST_METHOD(Test_Encode)
        {
            Assert::IsTrue(base::base64::Encode("Man") == "TWFu");
            Assert::IsTrue(base::base64::Encode("hello world") == "aGVsbG8gd29ybGQ=");
            Assert::IsTrue(base::base64::Encode("hello w") == "aGVsbG8gdw==");
            Assert::IsTrue(base::base64::Encode(u8"hello 世界") == "aGVsbG8g5LiW55WM");
        }

        TEST_METHOD(Test_Decode)
        {
            Assert::IsTrue(base::base64::Decode("TWFu") == "Man");
            Assert::IsTrue(base::base64::Decode("aGVsbG8gd29ybGQ=") == "hello world");
            Assert::IsTrue(base::base64::Decode("aGVsbG8gdw==") == "hello w");
            Assert::IsTrue(base::base64::Decode("aGVsbG8g5LiW55WM") == u8"hello 世界");

            Assert::IsFalse(base::base64::Decode("世界") != "");
            Assert::IsFalse(base::base64::Decode("ab+-") != "");
        }
    };
}