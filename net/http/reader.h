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

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "net/http/response.h"
#include "net/socket/StreamSocket.h"

namespace net {
namespace http {

class Reader
{
public:
    Reader() {}
    ~Reader() {}

    void Reset(StreamSocket* s);
    int GetErrorCode() const;

    std::string ExtractStartLine();
    std::vector<std::string> ExtractHeaders(bool& error);
    std::string ExtractOneChunked();
    void ExtractChunkedMessage(std::shared_ptr<Response> response);
    void ExtractContentMessage(std::shared_ptr<Response> response);

    void ExtractRequestMessage(std::shared_ptr<Request> request);

protected:
    void ExtractRawMessage(const std::string& contentLength, std::string& message);

protected:
    std::string m_buffer;
    StreamSocket* m_stream = nullptr;
    int m_error = 0;
};

} // !namespace http
} // !namespace net