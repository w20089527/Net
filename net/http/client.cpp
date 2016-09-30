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

int LocateLine(const char* buffer, size_t offset, size_t len)
{
    if (!buffer || offset >= len)
        return -1;
    for (size_t  i = offset; i < len; ++i)
    {
        if ('\n' == buffer[i])
        {
            return i;
        }
            
    }
    return -1;
}

std::string ExtractStartLine(StreamSocket& s, std::string& remainingBuffer)
{
    char buffer[1024];
    std::string message;
    do
    {
        int len = s.Receive(buffer, 1024);
        if (len <= 0)
            return "";

        int linePos = LocateLine(buffer, 0, len);
        if (linePos < 0)
        {
            message += std::string(buffer, len);
            continue;
        }
        message += std::string(buffer, linePos);
        remainingBuffer = std::string(buffer + linePos + 1, len - linePos - 1);
        break;
    } while (true);
    if (!message.empty() && '\r' == message.back())
        message.pop_back();
    return message;
}

std::vector<std::string> ExtractHeaders(
    StreamSocket& s,
    std::string& remainingBuffer,
    bool& error)
{
    std::vector<std::string> headers;
    
    std::string message;
    auto pos = remainingBuffer.find("\r\n\r\n");
    if (std::string::npos != pos)
    {
        message = remainingBuffer.substr(0, pos);
        remainingBuffer.erase(0, pos + 4);
        goto end;
    }
    else if (base::strings::StartsWith(remainingBuffer, "\r\n"))
    {
        remainingBuffer.erase(0, 2);
        goto end;
    }
    else
        message = remainingBuffer;

    char buffer[1024];
    bool finished = false;
    do 
    {
        int len = s.Receive(buffer, 1024);
        if (len <= 0)
        {
            error = true;
            return headers;
        }
        int linePos = 0;
        int lastPos = 0;
        while (linePos >= 0)
        {
            linePos = LocateLine(buffer, lastPos, len);
            if (linePos < 0)
            {
                message += std::string(buffer + lastPos, len - lastPos);
                break;
            }
            linePos += 1;
            message += std::string(buffer + lastPos, linePos - lastPos);
            lastPos = linePos;
            if (message.size() == 1 || base::strings::StartsWith(message, "\r\n"))
            {
                message.clear();
                finished = true;
                break;
            }
            
            if ('\n' == message[message.length() - 2])
            {
                message.erase(message.length() - 2);
                finished = true;
                break;
            }
            else if (message.size() > 2
                && '\r' == message[message.length() - 2]
                && '\n' == message[message.length() - 3])
            {
                if (message.size() > 3 && '\r' == message[message.length() - 4])
                    message.erase(message.length() - 4);
                else
                    message.erase(message.length() - 3);
                finished = true;
                break;
            }
        }
        if (finished)
        {
            remainingBuffer = std::string(buffer + lastPos, len - lastPos);
            break;
        }
    } while (true);

end:
    error = false;
    headers = base::strings::Split(message, "\n");
    for (auto iter = headers.begin(); iter != headers.end();)
    {
        if (iter->empty())
            iter = headers.erase(iter);
        else
        {
            if (iter->back() == '\r')
                iter->pop_back();
            ++iter;
        }
    }
    return headers;
}

std::string ExtractOneChunked(StreamSocket& s, std::string& remainingBuffer)
{
    std::string message;
    int needSize = 0;
    auto pos = remainingBuffer.find('\n');

    auto RemoveLine = [&]() {
        if (base::strings::StartsWith(remainingBuffer, "\r\n"))
            remainingBuffer.erase(0, 2);
        else if (remainingBuffer.size() < 2)
        {
            char b[2];
            int l = s.Receive(b, 2);
            if (l > 0)
            {
                remainingBuffer += std::string(b);
                if (base::strings::StartsWith(remainingBuffer, "\r\n"))
                    remainingBuffer.erase(0, 2);
            }
        }
    };

    if (pos != std::string::npos)
    {
        try
        {
            auto lineSize = std::stoi(remainingBuffer.substr(0, pos), 0, 16);
            remainingBuffer.erase(0, pos + 1);
            if (lineSize <= 0)
                return "";
            if (remainingBuffer.size() >= (size_t)lineSize)
            {
                message = remainingBuffer.substr(0, lineSize);
                remainingBuffer.erase(0, lineSize);
                RemoveLine();
                return message;
            }
            needSize = lineSize - remainingBuffer.size();
        }
        catch (...)
        {
            return "";
        }
        
    }
    else
    {
        char buffer[1024];
        int len = s.Receive(buffer, 1024);
        if (len <= 0)
            return "";
        int line = LocateLine(buffer, 0, len);
        if (line < 0)
            return "";
        std::string chunkedSize = remainingBuffer + std::string(buffer, line);
        remainingBuffer = std::string(buffer + line + 1, len - line - 1);
        try
        {
            auto lineSize = std::stoi(chunkedSize, 0, 16);
            if (lineSize <= 0)
                return "";
            needSize = lineSize - remainingBuffer.size();
            if (needSize <= 0)
            {
                message = remainingBuffer.substr(0, lineSize);
                remainingBuffer.erase(0, lineSize);
                RemoveLine();
                return message;
            }
        }
        catch (...)
        {
            return "";
        }
    }

    if (needSize <= 0)
        return "";
    std::string chunkedSize;
    char* buffer = new char[needSize + 2]; // include '\r\n'

    int len = s.Receive(buffer, needSize);
    if (len <= 0)
    {
        delete[]buffer;
        return "";
    };
    message = remainingBuffer + std::string(buffer, needSize);
    delete[]buffer;
    return message;
}

void ExtractChunkedMessage(
    StreamSocket& s,
    std::shared_ptr<Response> response,
    std::string& remainingBuffer)
{
    std::string message;
    do 
    {
        auto chunked = ExtractOneChunked(s, remainingBuffer);
        if (chunked.empty())
            break;
        message += chunked;
    } while (true);
    response->SetBody(message);
}

std::shared_ptr<Response> ResponseReceived(StreamSocket& s, std::shared_ptr<Request> request)
{
    std::shared_ptr<Response> response;

    std::string remainingBuffer;
    std::string requestLine = ExtractStartLine(s, remainingBuffer);
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
    auto headers = ExtractHeaders(s, remainingBuffer, error);
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
            ExtractChunkedMessage(s, response, remainingBuffer);
            break;
        }

        // Normal message.
        auto contentLength = response->GetHeader("Content-length");
        try
        {
            std::string body;
            auto len = std::stoi(contentLength) - (int)remainingBuffer.size();
            if (len < 0)
                break;
            else if (0 == len)
                body = remainingBuffer;
            else
            {
                std::unique_ptr<char[]> buffer(new char[len]);
                int recvBytes = s.Receive(buffer.get(), len);
                if (recvBytes != len)
                    break;
                
                body = remainingBuffer + std::string(buffer.get(), len);
            }
            if (response->GetHeader("Content-Encoding").find("gzip") != std::string::npos)
                response->SetBody(base::zip::GDecompress(body));
            else
                response->SetBody(body);
        }
        catch (...)
        {
            break;
        }
        
    } while (0);
    
    return response;
}

} // !namespace anonymous

std::shared_ptr<Client> Client::Create(const std::chrono::seconds timeout /*= std::chrono::seconds(30)*/)
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
    m_connection.Close();
    if (!m_connection.Connect(
        SocketAddress(host, (uint16_t)request->GetUrl().GetPort()),
        m_timeout))
        return nullptr;
    m_connection.SetReuseAddress(true);
    m_connection.SetNoDelay(true);
    int len = m_connection.Send((const void*)message.c_str(), message.length());
    if (len != (int)message.length())
    {
        m_connection.Close();
        return nullptr;
    }

    return ResponseReceived(m_connection, request);
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

} // !namespace http
} // !namespace net