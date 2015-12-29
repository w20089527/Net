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

namespace net {
namespace http {

class Url
{
public:
    // The general URL form represented is:
    // <scheme>://<user>:<password>@<host>:<port>/<path>?<query>#<frag>
    //
    // Well, if the URL does not start with a slash after scheme, we interpret it as follows:
    // <scheme>:opaque[?query][#fragment]
    //
    // The raw URL can be absolute or relative.
    static Url Parse(const std::string& strRawUrl);

    std::string GetScheme() const { return m_strScheme; }
    void SetScheme(const std::string& strScheme) { m_strScheme = strScheme; }
    std::string GetOpaque() const { return m_strOpaque; }
    void SetOpaque(const std::string& strOpaque) { m_strOpaque = strOpaque; }
    std::string GetUserName() const { return m_strUserName; }
    void SetUserName(const std::string& strUserName) { m_strUserName = strUserName; }
    std::string GetPassword() const { return m_strPassword; }
    void SetPassword(const std::string& strPassword) { m_strPassword = strPassword; }
    std::string GetHost() const { return m_strHost; }
    void SetHost(const std::string& strHost) { m_strHost = strHost; }
    int GetPort() const { return m_iPort; }
    void SetPort(int iPort) { m_iPort = iPort; }
    std::string GetPath() const { return m_strPath; }
    void SetPath(const std::string& strPath) { m_strPath = strPath; }

    // RawQuery is encoded query values, without '?'
    // For example, http://www.abc.com?a=1&b=2, the 'a=1&b=2' is raw query.
    std::string GetRawQuery() const { return m_strRawQuery; }
    void SetRawQuery(const std::string& strRawQuery) { m_strRawQuery = strRawQuery; }
    std::string GetFragment() const { return m_strFragment; }
    void SetFragment(const std::string& strFragment) { m_strFragment = strFragment; }

    // Whether the URL is absolute.
    bool IsAbsolute() const { return !m_strScheme.empty(); }

    // Return the escaped path.
    std::string EscapedPath() const;

    // Resolve a URI reference to an absolute URI from an absolute base URI.
    // The |ref| can be absolute or relative. If |ref| is absolute, we just copy it.
    // If |ref| is relative, we will make a new URI based on the absolute base URI.
    Url ResolveReference(const Url& ref) const;

    // The |strRawUrl| can be absolute or relative.
    // Inside we will call Parse method and
    // pass the returned URL as |ref| into ResolveReference(const Url& ref).
    Url ResolveReference(const std::string& strRawUrl) const;

    // Here we may return one of two kind URIs.
    // If opaque is not empty, the return form is:
    // "scheme:opaque?query#fragment"
    //
    // Otherwise, we return the form as follows:
    // "<scheme>://<user>:<password>@<host>:<port>/<path>?<query>#<frag>"
    std::string ToString() const;

private:
    std::string m_strScheme;
    std::string m_strOpaque;
    std::string m_strUserName;
    std::string m_strPassword;
    std::string m_strHost;
    int         m_iPort = -1;
    std::string m_strPath;
    std::string m_strRawQuery;
    std::string m_strFragment;
}; // !class Url

} // !namespace http
} // !namespace net