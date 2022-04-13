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

#include <alibabacloud/oss/http/HttpMessage.h>

using namespace AlibabaCloud::OSS;

HttpMessage::HttpMessage() :
    headers_(),
    body_()
{
}

HttpMessage::HttpMessage(const HttpMessage &other) :
    headers_(other.headers_),
    body_(other.body_)
{
}

HttpMessage::HttpMessage(HttpMessage &&other)
{
    *this = std::move(other);
}

HttpMessage& HttpMessage::operator=(const HttpMessage &other)
{
    if (this != &other) {
        body_ = other.body_;
        headers_ = other.headers_;
    }
    return *this;
}

HttpMessage& HttpMessage::operator=(HttpMessage &&other)
{
    if (this != &other) {
        body_ = std::move(other.body_);
        headers_ = std::move(other.headers_);
    }
    return *this;
}

void HttpMessage::addHeader(const std::string & name, const std::string & value)
{
    setHeader(name, value);
}

void HttpMessage::setHeader(const std::string & name, const std::string & value)
{
    headers_[name] = value;
}

void HttpMessage::removeHeader(const std::string & name)
{
    headers_.erase(name);
}


bool HttpMessage::hasHeader(const std::string &name) const
{
    return  (headers_.find(name) != headers_.end()) ? true : false;
}

std::string HttpMessage::Header(const std::string & name) const
{
    auto it = headers_.find(name);
    if (it != headers_.end())
        return it->second;
    else    
        return std::string();
}

const HeaderCollection &HttpMessage::Headers() const
{
    return headers_;
}

HttpMessage::~HttpMessage()
{

}


