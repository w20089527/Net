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

#include "net/http/reader.h"

#include "net/base/escape.h"
#include "net/base/strings/string_utils.h"
#include "net/base/zip.h"
#include "net/http/utils.h"

namespace net {
namespace http {

namespace {

int LocateLine(const char* buffer, size_t offset, size_t len)
{
    if (!buffer || offset >= len)
        return -1;
    for (size_t i = offset; i < len; ++i)
    {
        if ('\n' == buffer[i])
        {
            return i;
        }
    }
    return -1;
}

} // !namespace anonymous

void Reader::Reset(StreamSocket * s)
{
    m_stream = s;
    m_buffer.clear();
}

int Reader::GetErrorCode() const
{
    return m_error;
}

std::string Reader::ExtractStartLine()
{
    char buffer[1024];
    std::string message;
    do
    {
        int len = m_stream->Receive(buffer, 1024);
        if (len <= 0)
        {
            m_error = WSAGetLastError();
            return "";
        }

        int linePos = LocateLine(buffer, 0, len);
        if (linePos < 0)
        {
            message += std::string(buffer, len);
            continue;
        }
        message += std::string(buffer, linePos);
        m_buffer = std::string(buffer + linePos + 1, len - linePos - 1);
        break;
    } while (true);
    if (!message.empty() && '\r' == message.back())
        message.pop_back();
    return message;
}

std::vector<std::string> Reader::ExtractHeaders(bool& error)
{
    std::vector<std::string> headers;

    std::string message;
    auto pos = m_buffer.find("\r\n\r\n");
    if (std::string::npos != pos)
    {
        message = m_buffer.substr(0, pos);
        m_buffer.erase(0, pos + 4);
        goto end;
    }
    else if (base::strings::StartsWith(m_buffer, "\r\n"))
    {
        m_buffer.erase(0, 2);
        goto end;
    }
    else
        message = m_buffer;

    char buffer[1024];
    bool finished = false;
    do
    {
        int len = m_stream->Receive(buffer, 1024);
        if (len <= 0)
        {
            error = true;
            m_error = WSAGetLastError();
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
            m_buffer = std::string(buffer + lastPos, len - lastPos);
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

std::string Reader::ExtractOneChunked()
{
    std::string message;
    int needSize = 0;
    auto pos = m_buffer.find('\n');

    auto RemoveLine = [&]() {
        if (base::strings::StartsWith(m_buffer, "\r\n"))
            m_buffer.erase(0, 2);
        else if (m_buffer.size() < 2)
        {
            char b[2];
            int l = m_stream->Receive(b, 2);
            if (l > 0)
            {
                m_buffer += std::string(b);
                if (base::strings::StartsWith(m_buffer, "\r\n"))
                    m_buffer.erase(0, 2);
            }
            else
                m_error = WSAGetLastError();
        }
    };

    if (pos != std::string::npos)
    {
        try
        {
            // std::stoi works well for "xxx/r".
            auto lineSize = std::stoi(m_buffer.substr(0, pos), 0, 16);
            m_buffer.erase(0, pos + 1);
            if (lineSize <= 0)
                return "";
            if (m_buffer.size() >= (size_t)lineSize)
            {
                message = m_buffer.substr(0, lineSize);
                m_buffer.erase(0, lineSize);
                RemoveLine();
                return message;
            }
            needSize = lineSize - m_buffer.size();
        }
        catch (...)
        {
            return "";
        }

    }
    else
    {
        char buffer[1024];
        int len = m_stream->Receive(buffer, 1024);
        if (len <= 0)
        {
            m_error = WSAGetLastError();
            return "";
        }
        int line = LocateLine(buffer, 0, len);
        if (line < 0)
            return "";
        std::string chunkedSize = m_buffer + std::string(buffer, line);
        m_buffer = std::string(buffer + line + 1, len - line - 1);
        try
        {
            auto lineSize = std::stoi(chunkedSize, 0, 16);
            if (lineSize <= 0)
                return "";
            needSize = lineSize - m_buffer.size();
            if (needSize <= 0)
            {
                message = m_buffer.substr(0, lineSize);
                m_buffer.erase(0, lineSize);
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
    std::unique_ptr<char[]> buffer(new char[needSize + 2]); // include '\r\n'

    message = m_buffer;
    int remainSize = needSize + 2;
    do
    {
        int len = m_stream->Receive(buffer.get(), remainSize);
        if (len < 0)
        {
            m_error = WSAGetLastError();
            return "";
        }
        if (len == 0)
            continue;
        message += std::string(buffer.get(), len);
        remainSize -= len;
        if (remainSize == 0)
            break;;
    } while (true);

    // Remove "\r\n".
    message.pop_back();
    message.pop_back();
    m_buffer.clear();
    return message;
}

void Reader::ExtractChunkedMessage(std::shared_ptr<Response> response)
{
    std::string message;
    do
    {
        auto chunked = ExtractOneChunked();
        if (chunked.empty())
            break;
        message += chunked;
    } while (true);

    if (response->GetHeader("Content-Encoding").find("gzip") != std::string::npos)
        response->SetBody(base::zip::GDecompress(message));
    else
        response->SetBody(message);
}

void Reader::ExtractContentMessage(std::shared_ptr<Response> response)
{
    auto contentLength = response->GetHeader("Content-length");
    std::string body;
    
    ExtractRawMessage(contentLength, body);

    if (response->GetHeader("Content-Encoding").find("gzip") != std::string::npos)
        response->SetBody(base::zip::GDecompress(body));
    else
        response->SetBody(body);
}

void Reader::ExtractRequestMessage(std::shared_ptr<Request> request)
{
    auto contentLength = request->GetHeader("Content-length");
    std::string body;

    ExtractRawMessage(contentLength, body);

    if (request->GetHeader("Content-Encoding").find("gzip") != std::string::npos)
        body = base::zip::GDecompress(body);
    request->SetBody(body);
    
    auto contentType = request->GetHeader("Content-Type");
    if (contentType.empty())
        return;
    base::strings::ToLowerSelf(contentType);
    if (contentType.find("application/x-www-form-urlencoded") == std::string::npos)
        return;
    Values formValues;
    ParseQueryForm(body, formValues);
    request->SetPostFormValues(formValues);
}

void Reader::ExtractRawMessage(const std::string& contentLength, std::string & message)
{
    int len = 0;
    try
    {
        len = std::stoi(contentLength) - (int)m_buffer.size();
    }
    catch(...)
    {
        return;
    }
    
    if (len < 0)
        return;
    else if (0 == len)
        message = m_buffer;
    else
    {
        std::unique_ptr<char[]> buffer(new char[len]);
        int remainSize = len;
        message = m_buffer;
        do
        {
            int recvBytes = m_stream->Receive(buffer.get(), remainSize);
            if (recvBytes < 0)
            {
                m_error = WSAGetLastError();
                return;
            }
            if (recvBytes == 0)
                continue;
            message += std::string(buffer.get(), recvBytes);
            remainSize -= recvBytes;
            if (0 == remainSize)
                break;
        } while (true);
    }
}

} // !namespace http
} // !namespace net