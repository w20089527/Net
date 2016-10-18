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

#include "net/http/request.h"
#include "net/http/response.h"
#include "net/socket/StreamSocket.h"

namespace net {
namespace http {

class Client
{
public:
    ~Client() {}

    // Create makes an instance of Client
    // |timeout| represents the socket connect timeout.
    static std::shared_ptr<Client> Create(const std::chrono::seconds timeout = std::chrono::seconds(60));

    // Do sends a request to server.
    std::shared_ptr<Response> Do(std::shared_ptr<Request> request);

    // Get does a GET request for the specified url.
    std::shared_ptr<Response> Get(const std::string& url);

    // Head does a HEAD request.
    std::shared_ptr<Response> Head(const std::string& url);

    // Post issues a POST to the specified url.
    // |bodyType| represents the Content-Type.
    // |body| is the content to send.
    std::shared_ptr<Response> Post(const std::string& url, const std::string& bodyType, const std::string& body);

    // PostForm sends the key-value pairs to a server.
    std::shared_ptr<Response> PostForm(const std::string& url, const Values& data);

private:
    Client(const std::chrono::seconds timeout) : m_timeout(timeout) {}

    std::shared_ptr<Response> Send(std::shared_ptr<Request> request);
    std::shared_ptr<Response> DoFollowingRedirects(std::shared_ptr<Request> request);

private:
    StreamSocket m_connection;
    std::chrono::seconds m_timeout;
};

} // !namespace http
} // !namespace net