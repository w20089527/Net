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
#include <tuple>
#include <vector>

#include "net/base/url.h"
#include "net/http/common.h"
#include "net/http/cookie.h"
#include "net/http/httpdefs.h"

namespace net {
namespace http {

class Request
    : public CommonRequestResponse
{
public:
    ~Request() {}

    // NewRequest creates a request based on a method, URL and optional body.
    static std::shared_ptr<Request> NewRequest(const std::string& method, const std::string& url);

    std::string GetMethod() const;
    void SetMethod(const std::string& method);

    const Url& GetUrl();
    void SetUrl(const Url& url);

    std::string GetHost() const;
    void SetHost(const std::string& host);

    std::string GetRemoteAddress() const;
    void SetRemoteAddress(const std::string& remoteAddress);

    // AddCookie adds a cookie to the request's Cookie header.
    void AddCookie(const Cookie& cookie);

    // GetCookie gets the cookie by its name from the request's Cookie header.
    std::shared_ptr<Cookie> GetCookie(const std::string& name) const;

    // GetCookies gets all the cookies from the request's Cookie header.
    std::vector<std::shared_ptr<Cookie>> GetCookies() const;

    // BasicAuth returns the username and password provided in the request's Authorization header.
    // The tuple<0>: username
    // The tuple<1>: password
    // The tuple<2>: whether present and valid
    std::tuple<std::string, std::string, bool> BasicAuth() const;

    // SetBasicAuth sets the request's Authorization header to user HTTP Basic Authorization
    // with the provided username and password.
    void SetBasicAuth(const std::string& username, const std::string& password);

    // SetForm keeps the query string parameters.
    void SetForm(const Values& form);

    // SetPostForm keeps the the named component of POST or PUT request body.
    void SetPostForm(const Values& form);

    // FormValue returns the first value from the named component of the query.
    // POST and PUT body parameters take precedence over URL query string values.
    // If the key is not present, the empty string will be given.
    std::string FormValue(const std::string& key) const;

    // PostFormValue returns the first value from the named component of the POST or PUT request body.
    // The URL query parameters will be ignored.
    // If the key is not present, the empty string will be given.
    std::string PostFormValue(const std::string& key) const;

    // Referer returns the referer URL, if sent in the request.
    std::string Referer() const;

    // UserAgent returns the client's User-Agent.
    std::string UserAgent() const;

private:
    Request() {}

private:
    std::string m_method;
    Url m_url;
    std::string m_host;
    Values m_form;
    Values m_postForm;
    std::string m_remoteAddress;
};

} // !namespace http
} // !namespace net