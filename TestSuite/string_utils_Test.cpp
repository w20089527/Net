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
            std::string strData = "Hello, ����";
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
            strSearch = "HELLO, ��";
            Assert::IsTrue(base::strings::StartsWith(strData, strSearch, true));
        }

        TEST_METHOD(Test_EndsWith)
        {
            std::string strData = "Hello, ����";
            std::string strSearch = " ����";
            Assert::IsTrue(base::strings::EndsWith(strData, strSearch));
            strSearch = "O, ����";
            Assert::IsTrue(base::strings::EndsWith(strData, strSearch, true));
            strSearch = "Helloxxx";
            Assert::IsTrue(base::strings::EndsWith(strData, strSearch) == false);
            strSearch = "";
            Assert::IsTrue(base::strings::EndsWith(strData, strSearch));
            strSearch = strData;
            Assert::IsTrue(base::strings::EndsWith(strData, strSearch));
            strSearch = "HELLO, ����";
            Assert::IsTrue(base::strings::EndsWith(strData, strSearch, true));
        }

        TEST_METHOD(Test_ToLower_ToUpper)
        {
            std::string str = "Hello, ����";
            Assert::IsTrue(base::strings::ToLower(str) == "hello, ����");
            Assert::IsTrue(base::strings::ToUpper(str) == "HELLO, ����");
        }

        TEST_METHOD(Test_ToLowerSelf_ToUpperSelf)
        {
            std::string str = "Hello, ����";
            base::strings::ToLowerSelf(str);
            Assert::IsTrue("hello, ����" == str);
            base::strings::ToUpperSelf(str);
            Assert::IsTrue("HELLO, ����" == str);
        }

        TEST_METHOD(Test_TrimLeftSelf_TrimRightSelf_TrimSelf)
        {
            std::string str = " Hello, ����  ";
            base::strings::TrimLeftSelf(str, " ");
            Assert::IsTrue("Hello, ����  " == str);
            base::strings::TrimLeftSelf(str, "Helo, ");
            Assert::IsTrue("����  " == str);
            base::strings::TrimLeftSelf(str, "��");
            Assert::IsTrue("��  " == str);

            str = " Hello, ����  ";
            base::strings::TrimRightSelf(str, " ");
            Assert::IsTrue(" Hello, ����" == str);
            base::strings::TrimRightSelf(str, "���� ");
            Assert::IsTrue(" Hello," == str);

            str = " Hello, ����  ";
            base::strings::TrimSelf(str, " ");
            Assert::IsTrue("Hello, ����" == str);
            base::strings::TrimSelf(str, "H��");
            Assert::IsTrue("ello, ��" == str);
        }

        TEST_METHOD(Test_TrimLeft_TrimRight_Trim)
        {
            std::string str = " Hello, ����  ";
            Assert::IsTrue("Hello, ����  " == base::strings::TrimLeft(str, " "));
            Assert::IsTrue("����  " == base::strings::TrimLeft(str, "Helo, "));

            Assert::IsTrue(" Hello, ����" == base::strings::TrimRight(str, " "));
            Assert::IsTrue(" Hello," == base::strings::TrimRight(str, "���� "));

            Assert::IsTrue("Hello, ����" == base::strings::Trim(str, " "));
            Assert::IsTrue("ello, ��" == base::strings::Trim(str, "H�� "));
        }

        TEST_METHOD(Test_TrimLeftSpaceSelf_TrimRightSpaceSelf_TrimSpaceSelf)
        {
            #pragma warning(push)
            #pragma warning(disable: 4309)

            std::string str = " Hello, ����  ";
            
            base::strings::TrimLeftSpaceSelf(str);
            Assert::IsTrue("Hello, ����  " == str);
            base::strings::TrimRightSpaceSelf(str);
            Assert::IsTrue("Hello, ����" == str);
            str = " Hello, ����  ";
            base::strings::TrimSpaceSelf(str);
            Assert::IsTrue("Hello, ����" == str);

            str.push_back(0x85);
            str.push_back(0xA0);
            Assert::IsTrue("Hello, ����" != str);
            base::strings::TrimRightSpaceSelf(str);
            Assert::IsTrue("Hello, ����" == str);

            #pragma warning(pop)
        }

        TEST_METHOD(Test_TrimLeftSpace_TrimRightSpace_TrimSpace)
        {
            #pragma warning(push)
            #pragma warning(disable: 4309)

            std::string str = " Hello, ����  ";
            Assert::IsTrue("Hello, ����  " == base::strings::TrimLeftSpace(str));
            Assert::IsTrue(" Hello, ����" == base::strings::TrimRightSpace(str));
            Assert::IsTrue("Hello, ����" == base::strings::TrimSpace(str));

            str.push_back(0x85);
            str.push_back(0xA0);
            Assert::IsTrue(" Hello, ����  " != str);
            Assert::IsTrue(" Hello, ����" == base::strings::TrimRightSpace(str));
            Assert::IsTrue("Hello, ����" == base::strings::TrimSpace(str));

            #pragma warning(pop)
        }

        TEST_METHOD(Test_TrimPrefixSelf_TrimSuffixSelf)
        {
            std::string str = " hellohelloHello, ��������";
            base::strings::TrimPrefixSelf(str, " ");
            Assert::IsTrue("hellohelloHello, ��������" == str);
            base::strings::TrimPrefixSelf(str, "hello", false, true);
            Assert::IsTrue("Hello, ��������" == str);
            base::strings::TrimPrefixSelf(str, "Hello", true);
            Assert::IsTrue(", ��������" == str);
            str = "hellohelloHello, ��������";
            base::strings::TrimPrefixSelf(str, "hello", true, true);
            Assert::IsTrue(", ��������" == str);

            str = "Hello, ��������helloHello";
            base::strings::TrimSuffixSelf(str, "Hello");
            Assert::IsTrue("Hello, ��������hello" == str);
            str = "Hello, ��������helloHelloHello";
            base::strings::TrimSuffixSelf(str, "Hello", false, true);
            Assert::IsTrue("Hello, ��������hello" == str);
            str = "Hello, ��������helloHelloHello";
            base::strings::TrimSuffixSelf(str, "hello", true);
            Assert::IsTrue("Hello, ��������helloHello" == str);
            str = "Hello, ��������helloHelloHello";
            base::strings::TrimSuffixSelf(str, "hello", true, true);
            Assert::IsTrue("Hello, ��������" == str);
            base::strings::TrimSuffixSelf(str, "����", false, true);
            Assert::IsTrue("Hello, " == str);
        }

        TEST_METHOD(Test_TrimPrefix_TrimSuffix)
        {
            std::string str = "hellohelloHello, ��������";
            Assert::IsTrue("helloHello, ��������" == base::strings::TrimPrefix(str, "hello"));
            Assert::IsTrue("Hello, ��������" == base::strings::TrimPrefix(str, "hello", false, true));
            Assert::IsTrue("helloHello, ��������" == base::strings::TrimPrefix(str, "Hello", true));
            Assert::IsTrue(", ��������" == base::strings::TrimPrefix(str, "hello", true, true));

            str = "Hello, ��������helloHello";
            Assert::IsTrue("Hello, ��������hello" == base::strings::TrimSuffix(str, "Hello"));
            str = "Hello, ��������helloHelloHello";
            Assert::IsTrue("Hello, ��������hello" == base::strings::TrimSuffix(str, "Hello", false, true));
            Assert::IsTrue("Hello, ��������helloHello" == base::strings::TrimSuffix(str, "hello", true));
            Assert::IsTrue("Hello, ��������" == base::strings::TrimSuffix(str, "hello", true, true));
            str = "Hello, ��������";
            Assert::IsTrue("Hello, " == base::strings::TrimSuffix(str, "����", false, true));
        }
    };
}