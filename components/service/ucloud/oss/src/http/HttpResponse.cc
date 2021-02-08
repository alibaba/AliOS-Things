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

#include <alibabacloud/oss/http/HttpResponse.h>

namespace
{
    #define INVALID_STATUS_CODE -1
}

using namespace AlibabaCloud::OSS;

HttpResponse::HttpResponse(const std::shared_ptr<HttpRequest> & request) :
    HttpMessage(),
    request_(request),
    statusCode_(INVALID_STATUS_CODE)
{
}

HttpResponse::~HttpResponse()
{
}

const HttpRequest & HttpResponse::request() const
{
    return *request_.get();
}

void HttpResponse::setStatusCode(int code)
{
    statusCode_ = code;
}

int HttpResponse::statusCode() const
{
    return statusCode_;
}

void HttpResponse::setStatusMsg(std::string &msg)
{
    statusMsg_ = msg;
}

void HttpResponse::setStatusMsg(const char *msg)
{
    statusMsg_ = msg;
}

std::string HttpResponse::statusMsg() const
{
    return statusMsg_;
}
