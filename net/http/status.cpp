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

#include <map>

#include "net/http/status.h"

namespace net {
namespace http {

const std::map<Status, std::string> StatusTextMap {
    { Status::Continue, "Continue" },
    { Status::SwitchingProtocols, "Switching Protocols" },
    { Status::Processing, "Processing" },

    { Status::OK, "OK" },
    { Status::Created, "Created" },
    { Status::Accepted, "Accepted" },
    { Status::NonAuthoritativeInfo, "Non-Authoritative Information" },
    { Status::NoContent, "No Content" },
    { Status::ResetContent, "Reset Content" },
    { Status::PartialContent, "Partial Content" },
    { Status::MultiStatus, "Multi-Status" },
    { Status::AlreadyReported, "Already Reported" },
    { Status::IMUsed, "IM Used" },

    { Status::MultipleChoices, "Multiple Choices" },
    { Status::MovedPermanently, "Moved Permanently" },
    { Status::Found, "Found" },
    { Status::SeeOther, "See Other" },
    { Status::NotModified, "Not Modified" },
    { Status::UseProxy, "Use Proxy" },
    { Status::TemporaryRedirect, "Temporary Redirect" },
    { Status::PermanentRedirect, "Permanent Redirect" },

    { Status::BadRequest, "Bad Request" },
    { Status::Unauthorized, "Unauthorized" },
    { Status::PaymentRequired, "Payment Required" },
    { Status::Forbidden, "Forbidden" },
    { Status::NotFound, "Not Found" },
    { Status::MethodNotAllowed, "Method Not Allowed" },
    { Status::NotAcceptable, "Not Acceptable" },
    { Status::ProxyAuthRequired, "Proxy Authentication Required" },
    { Status::RequestTimeout, "Request Timeout" },
    { Status::Conflict, "Conflict" },
    { Status::Gone, "Gone" },
    { Status::LengthRequired, "Length Required" },
    { Status::PreconditionFailed, "Precondition Failed" },
    { Status::RequestEntityTooLarge, "Request Entity Too Large" },
    { Status::RequestURITooLong, "Request URI Too Long" },
    { Status::UnsupportedMediaType, "Unsupported Media Type" },
    { Status::RequestedRangeNotSatisfiable, "Requested Range Not Satisfiable" },
    { Status::ExpectationFailed, "Expectation Failed" },
    { Status::Teapot, "I'm a teapot" },
    { Status::UnprocessableEntity, "Unprocessable Entity" },
    { Status::Locked, "Locked" },
    { Status::FailedDependency, "Failed Dependency" },
    { Status::UpgradeRequired, "Upgrade Required" },
    { Status::PreconditionRequired, "Precondition Required" },
    { Status::TooManyRequests, "Too Many Requests" },
    { Status::RequestHeaderFieldsTooLarge, "Request Header Fields Too Large" },
    { Status::UnavailableForLegalReasons, "Unavailable For Legal Reasons" },

    { Status::InternalServerError, "Internal Server Error" },
    { Status::NotImplemented, "Not Implemented" },
    { Status::BadGateway, "Bad Gateway" },
    { Status::ServiceUnavailable, "Service Unavailable" },
    { Status::GatewayTimeout, "Gateway Timeout" },
    { Status::HTTPVersionNotSupported, "HTTP Version Not Supported"},
    { Status::VariantAlsoNegotiates, "Variant Also Negotiates" },
    { Status::InsufficientStorage, "Insufficient Storage" },
    { Status::LoopDetected, "Loop Detected" },
    { Status::NotExtended, "Not Extended" },
    { Status::NetworkAuthenticationRequired, "Network Authentication Required" },
};
        
std::string StatusText(Status code)
{
    auto v = StatusTextMap.find(code);
    if (v == StatusTextMap.end())
        return "???";
    return v->second;
}

} // !namespace http
} // !namespace net
