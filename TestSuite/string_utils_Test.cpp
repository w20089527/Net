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
#include "net/base/strings/string_utils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestSuite
{
    TEST_CLASS(string_utils_Test)
    {
    public:
        TEST_METHOD(Test_StartsWith)
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

        TEST_METHOD(Test_EndsWith)
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

        TEST_METHOD(Test_ToLower_ToUpper)
        {
            std::string str = "Hello, 世界";
            Assert::IsTrue(base::strings::ToLower(str) == "hello, 世界");
            Assert::IsTrue(base::strings::ToUpper(str) == "HELLO, 世界");
        }

        TEST_METHOD(Test_ToLowerSelf_ToUpperSelf)
        {
            std::string str = "Hello, 世界";
            base::strings::ToLowerSelf(str);
            Assert::IsTrue("hello, 世界" == str);
            base::strings::ToUpperSelf(str);
            Assert::IsTrue("HELLO, 世界" == str);
        }

        TEST_METHOD(Test_TrimLeftSelf_TrimRightSelf_TrimSelf)
        {
            std::string str = " Hello, 世界  ";
            base::strings::TrimLeftSelf(str, " ");
            Assert::IsTrue("Hello, 世界  " == str);
            base::strings::TrimLeftSelf(str, "Helo, ");
            Assert::IsTrue("世界  " == str);
            base::strings::TrimLeftSelf(str, "世");
            Assert::IsTrue("界  " == str);

            str = " Hello, 世界  ";
            base::strings::TrimRightSelf(str, " ");
            Assert::IsTrue(" Hello, 世界" == str);
            base::strings::TrimRightSelf(str, "界世 ");
            Assert::IsTrue(" Hello," == str);

            str = " Hello, 世界  ";
            base::strings::TrimSelf(str, " ");
            Assert::IsTrue("Hello, 世界" == str);
            base::strings::TrimSelf(str, "H界");
            Assert::IsTrue("ello, 世" == str);
        }

        TEST_METHOD(Test_TrimLeft_TrimRight_Trim)
        {
            std::string str = " Hello, 世界  ";
            Assert::IsTrue("Hello, 世界  " == base::strings::TrimLeft(str, " "));
            Assert::IsTrue("世界  " == base::strings::TrimLeft(str, "Helo, "));

            Assert::IsTrue(" Hello, 世界" == base::strings::TrimRight(str, " "));
            Assert::IsTrue(" Hello," == base::strings::TrimRight(str, "界世 "));

            Assert::IsTrue("Hello, 世界" == base::strings::Trim(str, " "));
            Assert::IsTrue("ello, 世" == base::strings::Trim(str, "H界 "));
        }

        TEST_METHOD(Test_TrimLeftSpaceSelf_TrimRightSpaceSelf_TrimSpaceSelf)
        {
            #pragma warning(push)
            #pragma warning(disable: 4309)

            std::string str = " Hello, 世界  ";
            
            base::strings::TrimLeftSpaceSelf(str);
            Assert::IsTrue("Hello, 世界  " == str);
            base::strings::TrimRightSpaceSelf(str);
            Assert::IsTrue("Hello, 世界" == str);
            str = " Hello, 世界  ";
            base::strings::TrimSpaceSelf(str);
            Assert::IsTrue("Hello, 世界" == str);

            str.push_back(0x85);
            str.push_back(0xA0);
            Assert::IsTrue("Hello, 世界" != str);
            base::strings::TrimRightSpaceSelf(str);
            Assert::IsTrue("Hello, 世界" == str);

            #pragma warning(pop)
        }

        TEST_METHOD(Test_TrimLeftSpace_TrimRightSpace_TrimSpace)
        {
            #pragma warning(push)
            #pragma warning(disable: 4309)

            std::string str = " Hello, 世界  ";
            Assert::IsTrue("Hello, 世界  " == base::strings::TrimLeftSpace(str));
            Assert::IsTrue(" Hello, 世界" == base::strings::TrimRightSpace(str));
            Assert::IsTrue("Hello, 世界" == base::strings::TrimSpace(str));

            str.push_back(0x85);
            str.push_back(0xA0);
            Assert::IsTrue(" Hello, 世界  " != str);
            Assert::IsTrue(" Hello, 世界" == base::strings::TrimRightSpace(str));
            Assert::IsTrue("Hello, 世界" == base::strings::TrimSpace(str));

            #pragma warning(pop)
        }

        TEST_METHOD(Test_TrimPrefixSelf_TrimSuffixSelf)
        {
            std::string str = " hellohelloHello, 世界世界";
            base::strings::TrimPrefixSelf(str, " ");
            Assert::IsTrue("hellohelloHello, 世界世界" == str);
            base::strings::TrimPrefixSelf(str, "hello", false, true);
            Assert::IsTrue("Hello, 世界世界" == str);
            base::strings::TrimPrefixSelf(str, "Hello", true);
            Assert::IsTrue(", 世界世界" == str);
            str = "hellohelloHello, 世界世界";
            base::strings::TrimPrefixSelf(str, "hello", true, true);
            Assert::IsTrue(", 世界世界" == str);

            str = "Hello, 世界世界helloHello";
            base::strings::TrimSuffixSelf(str, "Hello");
            Assert::IsTrue("Hello, 世界世界hello" == str);
            str = "Hello, 世界世界helloHelloHello";
            base::strings::TrimSuffixSelf(str, "Hello", false, true);
            Assert::IsTrue("Hello, 世界世界hello" == str);
            str = "Hello, 世界世界helloHelloHello";
            base::strings::TrimSuffixSelf(str, "hello", true);
            Assert::IsTrue("Hello, 世界世界helloHello" == str);
            str = "Hello, 世界世界helloHelloHello";
            base::strings::TrimSuffixSelf(str, "hello", true, true);
            Assert::IsTrue("Hello, 世界世界" == str);
            base::strings::TrimSuffixSelf(str, "世界", false, true);
            Assert::IsTrue("Hello, " == str);
        }

        TEST_METHOD(Test_TrimPrefix_TrimSuffix)
        {
            std::string str = "hellohelloHello, 世界世界";
            Assert::IsTrue("helloHello, 世界世界" == base::strings::TrimPrefix(str, "hello"));
            Assert::IsTrue("Hello, 世界世界" == base::strings::TrimPrefix(str, "hello", false, true));
            Assert::IsTrue("helloHello, 世界世界" == base::strings::TrimPrefix(str, "Hello", true));
            Assert::IsTrue(", 世界世界" == base::strings::TrimPrefix(str, "hello", true, true));

            str = "Hello, 世界世界helloHello";
            Assert::IsTrue("Hello, 世界世界hello" == base::strings::TrimSuffix(str, "Hello"));
            str = "Hello, 世界世界helloHelloHello";
            Assert::IsTrue("Hello, 世界世界hello" == base::strings::TrimSuffix(str, "Hello", false, true));
            Assert::IsTrue("Hello, 世界世界helloHello" == base::strings::TrimSuffix(str, "hello", true));
            Assert::IsTrue("Hello, 世界世界" == base::strings::TrimSuffix(str, "hello", true, true));
            str = "Hello, 世界世界";
            Assert::IsTrue("Hello, " == base::strings::TrimSuffix(str, "世界", false, true));
        }
    };
}