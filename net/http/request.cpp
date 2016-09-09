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
// 

#include "net/base/base64.h"
#include "net/base/strings/string_utils.h"
#include "net/http/request.h"

namespace net {
namespace http {

namespace {

bool IsValidMethod(const std::string& method)
{
    for each (char c in method)
    {
        if (c <= 32 || c >= 127)
        {
            return false;
        }
    }
    return true;
}

std::vector<std::shared_ptr<Cookie>> ParseCookies(
    const Header& header, const std::string& name)
{
    std::vector<std::shared_ptr<Cookie>> cookies;
    auto c = header.find("Cookie");
    if (c == header.end())
    {
        return cookies;
    }

    auto vlist = base::strings::Split(c->second, ";");
    for (auto item : vlist)
    {
        if (base::strings::TrimSpace(item).empty())
        {
            continue;
        }
        auto kv = base::strings::SplitN(item, "=", 2);
        if (kv.size() == 0)
            continue;
        auto k = base::strings::TrimSpace(kv[0]);
        if (k.empty())
            continue;
        std::shared_ptr<Cookie> cookie;
        if (name.empty() || k == name)
        {
            cookie = std::make_shared<Cookie>();
            cookie->Name = name;
        }
        else
        {
            continue;
        }
        if (kv.size() == 2)
        {
            cookie->Value = base::strings::TrimSpace(kv[1]);
        }
        cookies.push_back(cookie);
    }
    return cookies;
} // !ParseCookies

void ParseQuery(const std::string& rawQuery, Values& form)
{
    auto vlist = base::strings::Split(rawQuery, "&");
    for (auto item : vlist)
    {
        auto kv = base::strings::SplitN(item, "=", 2);
        if (kv.size() == 0)
            continue;
        auto key = base::strings::TrimSpace(kv[0]);
        if (key.empty())
            continue;
        std::string value;
        if (kv.size() == 2)
            value = base::strings::TrimSpace(kv[1]);
        form.emplace(key, value);
    }
} // !ParseQuery

} // !namespace anonymous

std::shared_ptr<Request> Request::NewRequest(
    const std::string & method,
    const std::string & url,
    const std::string & body)
{
    std::string validMethod(method);
    if (base::strings::TrimSpace(validMethod).empty())
    {
        validMethod = "GET";
    }
    if (!IsValidMethod(validMethod))
    {
        return nullptr;
    }
    auto u = Url::Parse(url);
    if (u.GetHost().empty())
    {
        return nullptr;
    }
    std::shared_ptr<Request> request(new Request());
    request->SetMethod(validMethod);
    request->SetProto(1, 1);
    request->SetUrl(u);
    request->SetBody(body);
    request->SetHost(u.HostPort());
    return request;
}

std::shared_ptr<Request> Request::ReadRequest(const std::string& buffer)
{
    // TODO: we need to parse the request from |buffer|.
    std::shared_ptr<Request> request(new Request());
    return request;
}

std::string Request::GetHeader(const std::string & key) const
{
    auto v = m_header.find(key);
    if (v == m_header.end())
    {
        return "";
    }
    return v->second;
}

std::vector<std::string> Request::GetHeaders(const std::string & key) const
{
    std::vector<std::string> valueList;
    auto vlist = m_header.equal_range(key);
    for (auto iter = vlist.first; iter != vlist.second; ++iter)
    {
        valueList.push_back(iter->second);
    }
    return valueList;
}

Header Request::GetHeaders() const
{
    return m_header;
}

void Request::SetHeader(const Header & header)
{
    m_header = header;
}

void Request::SetHeader(const std::string & key, const std::string & value)
{
    auto validKey = base::strings::TrimSpace(key);
    if (validKey.empty())
    {
        return;
    }
    auto v = m_header.find(validKey);
    if (v != m_header.end())
    {
        v->second = value;
    }
    else
    {
        m_header.emplace(validKey, value);
    }
}

std::string Request::GetMethod() const
{
    return m_method;
}

void Request::SetMethod(const std::string & method)
{
    m_method = base::strings::ToUpper(method);
}

const Url & Request::GetUrl()
{
    return m_url;
}

void Request::SetUrl(const Url & url)
{
    m_url = url;
}

std::string Request::GetProto() const
{
    return "HTTP/" + std::to_string(m_protoMajor) + "." + std::to_string(m_protoMinor);
}

void Request::SetProto(int protoMajor, int protoMinor)
{
    m_protoMajor = protoMajor;
    m_protoMinor = protoMinor;
}

std::string Request::GetBody() const
{
    return m_body;
}

void Request::SetBody(const std::string & body)
{
    m_body = body;
}

size_t Request::GetContentLength() const
{
    return m_body.length();
}

bool Request::GetClose() const
{
    return m_close;
}

void Request::SetClose(bool close)
{
    m_close = close;
}

std::string Request::GetHost() const
{
    return m_host;
}

void Request::SetHost(const std::string & host)
{
    m_host = host;
}

std::string Request::GetRemoteAddress() const
{
    return m_remoteAddress;
}

void Request::SetRemoteAddress(const std::string & remoteAddress)
{
    m_remoteAddress = remoteAddress;
}

void Request::AddCookie(const Cookie & cookie)
{
    std::string name = base::strings::TrimSpace(cookie.Name);
    std::string value = base::strings::TrimSpace(cookie.Value);
    if (name.empty() || value.empty())
        return;
    auto v = m_header.find("Cookie");
    if (v == m_header.end())
    {
        m_header.emplace("Cookie", name + "=" + value);
    }
    else
    {
        v->second += "; " + name + "=" + value;
    }
}

std::shared_ptr<Cookie> Request::GetCookie(const std::string & name) const
{
    auto vlist = ParseCookies(m_header, base::strings::TrimSpace(name));
    if (vlist.size() > 0)
    {
        return vlist[0];
    }
    return nullptr;
}

std::vector<std::shared_ptr<Cookie>> Request::GetCookies() const
{
    return ParseCookies(m_header, "");
}

std::tuple<std::string, std::string, bool> Request::BasicAuth() const
{
    do 
    {
        auto v = m_header.find("Authorization");
        if (v == m_header.end())
            break;
        auto vlist = base::strings::SplitN(v->second, " ", 2);
        if (vlist.size() != 2)
            break;
        if (!base::strings::Equal(vlist[0], "Basic", true))
            break;
        auto value = base::base64::Decode(vlist[1]);
        if (value.empty())
            break;

        auto auth = base::strings::SplitN(value, ":", 2);
        if (auth.size() == 2)
            return{ auth[0], auth[1], true };
        else
            return{ auth[0], "", true };
    } while (0);
    return{ "", "", false };
}

void Request::ParseForm()
{
    ParseQuery(m_url.GetRawQuery(), m_form);

    if ("POST" != m_method
        && "PUT" != m_method
        && "PATCH" != m_method)
    {
        return;
    }
    auto contentType = base::strings::TrimSpace(GetHeader("Content-Type"));
    if (contentType.empty())
        return;
    auto type = base::strings::SplitN(contentType, ";", 2)[0];
    type = base::strings::ToLower(type);
    if ("application/x-www-form-urlencoded" == type)
    {
        ParseQuery(m_body, m_postForm);
    }
    else if ("multipart/form-data" == type)
    {
        ParseMultipartForm();
    }
}

void Request::ParseMultipartForm()
{
}

void Request::SetBasicAuth(const std::string & username, const std::string & password)
{
    auto value = base::base64::Encode(username + ":" + password);
    SetHeader("Authorization", "Basic " + value);
}

std::string Request::FormValue(const std::string & key) const
{
    auto v = m_postForm.find(key);
    if (v != m_postForm.end())
        return v->second;
    v = m_form.find(key);
    if (v != m_form.end())
        return v->second;
    return "";
}

std::string Request::PostFormValue(const std::string & key) const
{
    auto v = m_postForm.find(key);
    if (v != m_postForm.end())
        return v->second;
    return "";
}

std::string Request::Referer() const
{
    return GetHeader("Referer");
}

std::string Request::UserAgent() const
{
    return GetHeader("User-Agent");
}

} // !namespace http
} // !namespace net