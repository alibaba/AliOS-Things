/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <alibabacloud/oss/http/HttpRequest.h>

using namespace AlibabaCloud::OSS;

std::string Http::MethodToString(Method method)
{
    static const char* name[] = {"GET", "HEAD", "POST", "PUT", "DELETE", 
        "CONNECT", "OPTIONS", "PATCH", "TRACE"};
    return name[(method - Http::Method::Get)];
}

std::string Http::SchemeToString(Scheme scheme)
{
    static const char* name[] = {"http", "https"};
    return name[scheme - Http::Scheme::HTTP];
}

const char* Http::ACCEPT = "Accept";
const char* Http::ACCEPT_CHARSET = "Accept-Charset";
const char* Http::ACCEPT_ENCODING = "Accept-Encoding";
const char* Http::ACCEPT_LANGUAGE = "Accept-Language";
const char* Http::AUTHORIZATION = "Authorization";
const char* Http::CACHE_CONTROL = "Cache-Control";
const char* Http::CONTENT_DISPOSITION = "Content-Disposition";
const char* Http::CONTENT_ENCODING = "Content-Encoding";
const char* Http::CONTENT_LENGTH = "Content-Length";
const char* Http::CONTENT_MD5 = "Content-MD5";
const char* Http::CONTENT_RANGE = "Content-Range";
const char* Http::CONTENT_TYPE = "Content-Type";
const char* Http::DATE = "Date";
const char* Http::EXPECT = "Expect";
const char* Http::EXPIRES = "Expires";
const char* Http::ETAG = "ETag";
const char* Http::LAST_MODIFIED = "Last-Modified";
const char* Http::RANGE = "Range";
const char* Http::USER_AGENT = "User-Agent";


HttpRequest::HttpRequest(Http::Method method) :
    HttpMessage(),
    method_(method),
    url_(),
    responseStreamFactory_(nullptr),
    hasCheckCrc64_(false),
    crc64Result_(0),
    transferedBytes_(0)
{
}

HttpRequest::~HttpRequest()
{
}

Http::Method HttpRequest::method() const
{
    return method_;
}


void HttpRequest::setMethod(Http::Method method)
{
    method_ = method;
}

void HttpRequest::setUrl(const Url &url)
{
    url_ = url;
}

Url HttpRequest::url()const
{
    return url_;
}
