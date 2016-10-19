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

#include "net/base/zip.h"
#include "net/http/client.h"
#include "net/http/status.h"

namespace net {
namespace http {

namespace {

bool ShouldRedirectGet(int status)
{
    Status code = (Status)status;
    switch (code)
    {
    case Status::MovedPermanently:
    case Status::Found:
    case Status::SeeOther:
    case Status::TemporaryRedirect:
        return true;
    }
    return false;
}

bool ShouldRedirectPost(int status)
{
    Status code = (Status)status;
    switch (code)
    {
    case Status::Found:
    case Status::SeeOther:
        return true;
    }
    return false;
}

bool ShouldRedirect(const std::string& method, int status)
{
    if ("GET" == method || "HEAD" == method)
        return ShouldRedirectGet(status);
    else if ("POST" == method || "PUT" == method)
        return ShouldRedirectPost(status);
    return false;
}

std::string ValuesToString(const Values& data)
{
    std::string body;
    for (auto iter = data.begin(); iter != data.end(); ++iter)
    {
        if (!body.empty())
        {
            body += "&";
        }
        body += iter->first + "=" + iter->second;
    }
    return body;
}

std::string RefererForUrl(const Url& lastUrl, const Url& newUrl)
{
    if (base::strings::Equal(lastUrl.GetScheme(), "https", true)
        && base::strings::Equal(newUrl.GetScheme(), "http", true))
    {
        return "";
    }
    std::string referer = lastUrl.ToString();
    if (!lastUrl.GetUser().empty())
    {
        auto pos1 = referer.find_first_of(lastUrl.GetUser());
        auto pos2 = referer.find_first_of('@');
        referer.replace(pos1, pos2 - pos1 + 1, "");
    }
    return referer;
}

std::string RequestLine(std::shared_ptr<Request> request)
{
    std::string method = request->GetMethod();
    if (method.empty())
        method = "GET";
    std::string uri = request->GetUrl().RequestURI();
    if (uri.empty())
        uri = "/";
    std::string proto = request->GetProto();
    if (proto.empty())
        proto = "HTTP/1.1";
    return method + " " + uri + " " + proto + "\r\n";
}

std::string RequestHeaders(std::shared_ptr<Request> request)
{
    if (!request->GetUrl().GetUser().empty())
        request->SetBasicAuth(request->GetUrl().GetUser(), request->GetUrl().GetPassword());
    auto headers = request->GetHeaders();
    std::string result;
    for (auto iter = headers.begin(); iter != headers.end(); ++iter)
    {
        result += iter->first + ": " + iter->second + "\r\n";
    }
    return result;
}

bool IsIpV4(const std::string& host)
{
    auto vlist = base::strings::Split(host, ".");
    if (vlist.size() != 4)
        return false;
    for (size_t i = 0; i < vlist.size(); ++i)
    {
        if (!base::strings::IsDigit(vlist[i]))
            return false;
        auto value = std::stoi(vlist[i]);
        if (value < 0 || value > 255)
            return false;
    }
    return true;
}

} // !namespace anonymous

std::shared_ptr<Client> Client::Create(const std::chrono::seconds timeout /*= std::chrono::seconds(60)*/)
{
    return std::shared_ptr<Client>(new Client(timeout));
}

std::shared_ptr<Response> Client::Do(std::shared_ptr<Request> request)
{
    if (!request)
        return nullptr;
    if (request->GetMethod() == "GET"
        || request->GetMethod() == "HEAD"
        || request->GetMethod() == "PUT"
        || request->GetMethod() == "POST")
    {
        return DoFollowingRedirects(request);
    }
    return Send(request);
}

std::shared_ptr<Response> Client::Get(const std::string & url)
{
    auto request = Request::Create("GET", url);
    if (!request)
        return nullptr;
    return DoFollowingRedirects(request);
}

std::shared_ptr<Response> Client::Head(const std::string & url)
{
    auto request = Request::Create("HEAD", url);
    if (!request)
        return nullptr;
    return DoFollowingRedirects(request);
}

std::shared_ptr<Response> Client::Post(const std::string & url, const std::string & bodyType, const std::string & body)
{
    auto request = Request::Create("POST", url, body);
    if (!request)
        return nullptr;
    request->SetHeader("Content-Type", bodyType);
    return DoFollowingRedirects(request);
}

std::shared_ptr<Response> Client::PostForm(const std::string & url, const Values & data)
{
    return Post(url, "application/x-www-form-urlencoded", ValuesToString(data));
}

std::shared_ptr<Response> Client::Send(std::shared_ptr<Request> request)
{
    if (!request)
        return nullptr;
    std::string message;
    message = RequestLine(request) + RequestHeaders(request) + "\r\n" + request->GetBody();
    std::string host = request->GetUrl().GetHost();
    if (!IsIpV4(host))
    {
        auto h = gethostbyname(host.c_str());
        if (!h)
            return nullptr;
        struct in_addr **addr_list = (struct in_addr **)h->h_addr_list;
        host = inet_ntoa(*addr_list[0]);
    }
    uint16_t port = (uint16_t)request->GetUrl().GetPort();
    do 
    {
        SocketAddress remoteAddress;
        if (m_connection.GetNativeHandle() != INVALID_SOCKET)
            remoteAddress = m_connection.GetForeignAddress();
        if (remoteAddress.GetHost() == host && remoteAddress.GetPort() == port)
            break;
        m_connection.Close();
        if (!m_connection.Connect(
            SocketAddress(host, port),
            m_timeout))
            return nullptr;
        m_connection.SetReuseAddress(true);
        m_connection.SetNoDelay(true);
        m_reader.Reset(&m_connection);
    } while (0);
    
    int len = m_connection.Send((const void*)message.c_str(), message.length());
    if (len != (int)message.length())
    {
        m_connection.Close();
        return nullptr;
    }

    return ResponseReceived(request);
}

std::shared_ptr<Response> Client::DoFollowingRedirects(std::shared_ptr<Request> request)
{
    if (!request)
        return nullptr;
    auto response = Send(request);
    if (!response || !ShouldRedirect(request->GetMethod(), response->GetStatusCode()))
        return response;

    std::shared_ptr<Request> lastRequest = request;
    int count = 10;
    while (--count > 0)
    {
        auto location = response->GetHeader("Location");
        if (location.empty())
            return nullptr;

        auto r = Request::Create(request->GetMethod(), location);
        if (!r)
            return nullptr;
        if (r->GetMethod() == "POST" || r->GetMethod() == "PUT")
            r->SetMethod("GET");
        std::string referer = RefererForUrl(lastRequest->GetUrl(), r->GetUrl());
        if (!referer.empty())
        {
            r->SetHeader("Referer", referer);
        }
        
        response = Send(r);
        if (!response)
            return nullptr;
        if (!ShouldRedirect(request->GetMethod(), response->GetStatusCode()))
            return response;
        lastRequest = r;
    }
    return response;
}

std::shared_ptr<net::http::Response> Client::ResponseReceived(std::shared_ptr<Request> request)
{
    std::shared_ptr<Response> response;

    std::string requestLine = m_reader.ExtractStartLine();
    auto vlist = base::strings::SplitN(requestLine, " ", 3);
    if (vlist.size() != 3)
        return nullptr;

    // Parse the start line.
    auto proto = base::strings::Split(vlist[0], "/");
    if (proto.size() != 2)
        return nullptr;

    try
    {
        auto protoNumber = base::strings::Split(proto[1], ".");
        int protoMajor = std::stoi(protoNumber[0]);
        int protoMinor = 0;
        if (protoNumber.size() == 2)
            protoMinor = std::stoi(protoNumber[1]);

        response = Response::Create();
        response->SetRequest(request);
        response->SetProto(protoMajor, protoMinor);
        response->SetStatusCode(std::stoi(vlist[1]));
        response->SetStatus(vlist[2]);
    }
    catch (...)
    {
        return nullptr;
    }

    // Parse headers.
    bool error = false;
    auto headers = m_reader.ExtractHeaders(error);
    if (error)
        return nullptr;
    for (auto h : headers)
    {
        auto kv = base::strings::SplitN(h, ":", 2);
        if (kv.size() != 2)
            continue;
        response->SetHeader(base::strings::TrimSpace(kv[0]), base::strings::TrimSpace(kv[1]));
    }

    // Extract response body.
    do
    {
        if (response->GetRequest()->GetMethod() == "HEAD")
            break;

        // Chunked message.
        if (response->GetHeader("Transfer-Encoding") == "chunked")
        {
            m_reader.ExtractChunkedMessage(response);
            break;
        }

        // Normal message.
        m_reader.ExtractContentMessage(response);

    } while (0);

    return response;
}

} // !namespace http
} // !namespace net