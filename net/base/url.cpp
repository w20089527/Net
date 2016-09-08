// The MIT License (MIT)
//
// Copyright(c) 2015-2016 huan.wang
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

#include <stack>

#include "net/base/escape.h"
#include "net/base/strings/string_utils.h"
#include "net/base/url.h"

namespace net {
namespace http {

namespace {

bool IsValidScheme(const std::string& strScheme)
{
    if (strScheme.empty())
        return false;
    if ((strScheme[0] >= '0' && strScheme[0] <= '9')
        || '+' == strScheme[0]
        || '-' == strScheme[0]
        || '.' == strScheme[0])
        return false;
    for (size_t i = 0; i < strScheme.length(); ++i)
    {
        if ('a' <= strScheme[i] && strScheme[i] <= 'z'
            || 'A' <= strScheme[i] && strScheme[i] <= 'Z'
            || '0' <= strScheme[i] && strScheme[i] <= '9'
            || '+' == strScheme[i]
            || '-' == strScheme[i]
            || '.' == strScheme[i])
            continue;
        return false;
    }
    return true;
}
bool ParseScheme(const std::string& strRawUrl,
                 std::string& strScheme, std::string& strRest)
{
    // Extract scheme.
    auto pos = strRawUrl.find(':');
    if (pos != std::string::npos)
    {
        // Missing protocol scheme, so the raw url is invalid.
        if (0 == pos)
            return false;
        strScheme = base::strings::ToLower(strRawUrl.substr(0, pos));
        // If it is not a valid scheme, we assume that the raw url is a path.
        if (IsValidScheme(strScheme))
        {
            strRest = strRawUrl.substr(pos + 1);
            return true;
        }
    }
    strScheme = "";
    strRest = strRawUrl;
    return true;
}
bool ParseAuthority(const std::string& strAuthority,
                    std::string& strUserName, std::string& strPassword,
                    std::string& strHost, int& iPort)
{
    if (strAuthority.empty())
        return false;
    auto spList = base::strings::Split(strAuthority, "@");
    if (spList.size() > 2
        || (spList.size() == 2 && spList[1].empty()))
        return false;

    std::string strHostPort;
    if (spList.size() == 2)
    {
        // Parse UserName:Password.
        auto spList1 = base::strings::Split(spList[0], ":");
        if (spList1.size() > 2)
            return false;
        strUserName = base::Unescape(spList1[0]);
        if (spList1.size() == 2)
            strPassword = base::Unescape(spList1[1]);
        strHostPort = spList[1];
    }
    else
        strHostPort = spList[0];
    // Parse Host:Port.
    auto spList1 = base::strings::Split(strHostPort, ":");
    if (spList1.size() > 2)
        return false;
    strHost = base::Unescape(spList1[0]);
    if (spList1.size() == 2)
    {
        if (base::strings::IsDigit(spList1[1]))
            iPort = std::stoi(spList1[1]);
        else
            return false;
    }

    return true;
}

} // !namespace

Url Url::Parse(const std::string& strRawUrl)
{
    // <scheme>://<user>:<password>@<host>:<port>/<path>?<query>#<frag>
    // <scheme>:opaque?query#fragment
    Url url;
    if (strRawUrl.empty())
        return url;
    do 
    {
        // Firstly, we will split fragment and query string.
        std::string strPrefix;
        auto spList = base::strings::Split(strRawUrl, "#");
        if (spList.size() > 2)
            break;
        strPrefix = spList[0];
        if (spList.size() == 2)
            url.m_strFragment = base::Unescape(spList[1]);
        spList = base::strings::Split(strPrefix, "?");
        if (spList.size() > 2)
            break;
        strPrefix = spList[0];
        if (spList.size() == 2)
            url.m_strRawQuery = spList[1];
        
        // Parse scheme.
        std::string strRest;
        if (!ParseScheme(strPrefix, url.m_strScheme, strRest))
            break;
        if (strRest.empty())
            break;
        
        // <scheme>:opaque
        if (!url.m_strScheme.empty() && strRest[0] != '/')
        {
            url.m_strOpaque = strRest;
            break;
        } // !if <scheme>:opaque

        // Invalid URL.
        if (!url.m_strScheme.empty()
            && !base::strings::StartsWith(strRest, "//"))
            break;

        // Relative path.
        if (url.m_strScheme.empty()
            && !base::strings::StartsWith(strRest, "//"))
        {
            url.m_strPath = base::Unescape(strRest);
            break;
        }

        // Secondly, we will parse the particular case.
        // <scheme>:///<path>
        if (strRest[2] == '/')
        {
            auto pos1 = strRest.find_first_not_of('/');
            if (std::string::npos == pos1)
                break;
            url.m_strPath = base::Unescape(strRest.substr(pos1 - 1));
            break;
        } // !if <scheme>:///<path>

        // <scheme>://<user>:<password>@<host>:<port>/<path>
        auto pos1 = strRest.find_first_not_of('/');
        if (std::string::npos == pos1)
            break;
        auto pos2 = strRest.find_first_of("/", pos1);
        std::string strAuthority;
        if (std::string::npos == pos2)
            strAuthority = strRest.substr(pos1);
        else
            strAuthority = strRest.substr(pos1, pos2 - pos1);
        if (!ParseAuthority(strAuthority, url.m_strUserName, url.m_strPassword,
                            url.m_strHost, url.m_iPort))
        {
            url.m_strUserName = "";
            url.m_strPassword = "";
            url.m_strHost = "";
            url.m_iPort = 0;
            break;
        }
        if (pos2 == std::string::npos)
        {
            url.m_strPath = "/";
            break;
        }
        url.m_strPath = base::Unescape(strRest.substr(pos2));
        
    } while (0);
    
    return url;
}

std::string Url::HostPort() const
{
    std::string hostPort;
    if (m_strHost.length() > 0)
    {
        hostPort = m_strHost;
    }
    if (m_iPort >= 0)
    {
        hostPort += ":" + std::to_string(m_iPort);
    }
    return hostPort;
}

std::string Url::EscapedPath() const
{
    return base::EscapeUrl(m_strPath);
}

Url Url::ResolveReference(const Url& ref) const
{
    if (ref.IsAbsolute())
        return ref;

    Url url;
    url.SetScheme(m_strScheme);
    url.SetUserName(m_strUserName);
    url.SetPassword(m_strPassword);
    url.SetHost(m_strHost);
    url.SetPort(m_iPort);
    url.SetRawQuery(ref.GetRawQuery());
    url.SetFragment(ref.GetFragment());

    auto pathDirList = base::strings::Split(m_strPath, "/");
    auto relDirList = base::strings::Split(ref.GetPath(), "/");
    std::stack<std::string> pathDirStack;
    for (auto v : pathDirList)
    {
        if (v.empty())
            continue;
        pathDirStack.push(v);
    }
    for (auto v : relDirList)
    {
        if (v.empty() || "." == v)
            continue;

        if (".." == v)
        {
            if (!pathDirStack.empty())
                pathDirStack.pop();
        }
        else
            pathDirStack.push(v);
    }
    if (pathDirStack.empty())
        url.SetPath("/");
    else
    {
        std::string strPath;
        while (!pathDirStack.empty())
        {
            strPath = pathDirStack.top() + "/" + strPath;
            pathDirStack.pop();
        }
        url.SetPath("/" + strPath);
    }
    return url;
}

Url Url::ResolveReference(const std::string& strRawUrl) const
{
    return ResolveReference(Parse(strRawUrl));
}

std::string Url::ToString() const
{
    std::string strUrl;
    std::string strTail;

    if (!m_strRawQuery.empty())
        strTail = "?" + m_strRawQuery;
    if (!m_strFragment.empty())
        strTail += "#" + base::EscapeUrlComponent(m_strFragment);

    if (!m_strScheme.empty())
        strUrl = m_strScheme + ":";

    // scheme:opaque?query#fragment
    if (!m_strOpaque.empty())
        return strUrl + m_strOpaque + strTail;

    // <scheme>://<user>:<password>@<host>:<port>/<path>?<query>#<frag>
    if (!strUrl.empty())
        strUrl += "//";
    if (!m_strUserName.empty())
    {
        strUrl += base::EscapeUrlComponent(m_strUserName);
        if (!m_strPassword.empty())
            strUrl += ":" + base::EscapeUrlComponent(m_strPassword);
        if (!m_strHost.empty())
            strUrl += "@";
    }
    if (!m_strHost.empty())
    {
        strUrl += base::EscapeUrlComponent(m_strHost);
        if (m_iPort >= 0)
            strUrl += ":" + std::to_string(m_iPort);
    }
    if (strUrl.empty() && m_strPath.empty())
        return "";
    strUrl += m_strPath.empty() ? "/" : EscapedPath();
    return strUrl + strTail;
}

} // !namespace http
} // !namespace net