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

#include <set>
#include <string>

namespace base {

// Escape |str| and |excludedSet| is a set of chars that will not be escaped.
// |bKeepEscaped| means whether %XX will be kept. If |bKeepEscaped| is false,
// the '%' of "%XX" will be escaped depending on |excludedSet|.
// Otherwise, it will be kept, no matter what |excludedSet|.
// We will replace space with plus('+') if |bUsePlus| is true.
std::string Escape(const std::string& str, const std::set<char>& excludedSet,
                   bool bKeepEscaped = false, bool bUsePlus = false);

// |str| is the source string which will be escaped.
// The escaped form is %XX. Alphanumerics and *+-./@_ will not be escaped.
// If |bKeepEscaped| is true, the %XX will remain as it is.
// Otherwise, %XX will be escaped.
std::string Escape(const std::string& str, bool bKeepEscaped = false);

// This method equals encodeURI in javascript.
// Alphanumerics and !#$&'()*+,-./:;=?@_~ will not be escaped.
std::string EscapeUrl(const std::string& strUrl, bool bUsePlus = false);

// This method equals encodeURIComponent in javascript.
// Alphanumerics and !'()*-._~ will not be escaped.
std::string EscapeUrlComponent(const std::string& strUrl, bool bUsePlus = false);

// Escapes application/x-www-form-urlencoded content.
// ?>=<;+'&%$#"![\]^`{|} will be escaped.
// Space is escaped as + if |bUsePlus| is true.
std::string EscapeUrlEncodedData(const std::string& strData, bool bUsePlus = false);

// Unescape |str| and if |bReplacePlus| is true, the + will be replaced with space.
// Substrings in |str| like %XX will be escaped and X is a hex digit.
std::string Unescape(const std::string& str, bool bReplacePlus = false);

// Escape HTML tags(<, >, &, ", ').
std::string EscapeForHTML(const std::string& str);

// Unescape &lt; &gt; &amp; &quot; &#39; for HTML tags.
std::string UnescapeForHTML(const std::string& str);

} // !namespace base