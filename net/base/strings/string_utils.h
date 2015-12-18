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
// 

#pragma once

#include <string>
#include <vector>

namespace base {
namespace strings {

bool Equal(const std::string& str1, const std::string& str2, bool bIgnoreCase = false);
bool StartsWith(const std::string& strData, const std::string& strSearch, bool bIgnoreCase = false);
bool EndsWith(const std::string& strData, const std::string& strSearch, bool bIgnoreCase = false);

std::string ToLower(const std::string& str);
std::string ToUpper(const std::string& str);
void ToLowerSelf(std::string& str);
void ToUpperSelf(std::string& str);

// The method with Self suffix will make changes on the source string.
// |strTrimChars| is a set of chars that will be trimmed.
void TrimLeftSelf(std::string& str, const std::string& strTrimChars);
void TrimRightSelf(std::string& str, const std::string& strTrimChars);
void TrimSelf(std::string& str, const std::string& strTrimChars);

// The method without Self suffix will make a new instance of trimmed string.
// And the source string will not be modified.
std::string TrimLeft(const std::string& str, const std::string& strTrimChars);
std::string TrimRight(const std::string& str, const std::string& strTrimChars);
std::string Trim(const std::string& str, const std::string& strTrimChars);

// The space including '\t', '\n', '\v', '\f', '\r', ' ', 0x85, 0xA0 will be trimmed.
void TrimLeftSpaceSelf(std::string& str);
void TrimRightSpaceSelf(std::string& str);
void TrimSpaceSelf(std::string& str);
std::string TrimLeftSpace(const std::string& str);
std::string TrimRightSpace(const std::string& str);
std::string TrimSpace(const std::string& str);

// |bRecursive| means whether we should trim a particular prefix or suffix recursively.
// For example, "hellohello world", we trim the "hello" prefix and if |bRecursive| is true,
// the result is " world". Otherwise it will be "hello world".
void TrimPrefixSelf(std::string& str, const std::string& strPrefix,
                    bool bIgnoreCase = false, bool bRecursive = false);
void TrimSuffixSelf(std::string& str, const std::string& strSuffix,
                    bool bIgnoreCase = false, bool bRecursive = false);
std::string TrimPrefix(const std::string& str, const std::string& strPrefix,
                       bool bIgnoreCase = false, bool bRecursive = false);
std::string TrimSuffix(const std::string& str, const std::string& strSuffix,
                       bool bIgnoreCase = false, bool bRecursive = false);

// |str| is the source string which will be split.
// |sep| is the separator which will be used to split the str.
// If |sep| is empty. we will split the |str| by each byte.
// |n| determines how many substrings will be returned:
//   n > 0 : at most n substrings; the last will be the unsplit remainder.
//   n == 0: will return an empty list.
//   n < 0 : equal to Split which will return all substrings.
// |bIgnoreCase| means whether we should ignore sep case.
std::vector<std::string> SplitN(const std::string& str, const std::string& sep, int n, bool bIgnoreCase = false);

// After means we will split the |str| after |sep|, and |sep| will be put into substrings.
std::vector<std::string> SplitAfterN(const std::string& str, const std::string& sep, int n, bool bIgnoreCase = false);
std::vector<std::string> Split(const std::string& str, const std::string& sep, bool bIgnoreCase = false);
std::vector<std::string> SplitAfter(const std::string& str, const std::string& sep, bool bIgnoreCase = false);

// Determines whether the |str| consists of digits.
// |bHex| determines whether the |str| is hexadecimal.
bool IsDigit(const std::string& str, bool bHex = false);

} // !namespace strings
} // !namespace base