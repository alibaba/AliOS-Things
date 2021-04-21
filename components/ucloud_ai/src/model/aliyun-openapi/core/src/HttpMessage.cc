/*
 * Copyright 1999-2019 Alibaba Cloud All rights reserved.
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

#include <algorithm>
#include <alibabacloud/core/HttpMessage.h>

namespace AlibabaCloud {

namespace {
#if defined(WIN32) && defined(_MSC_VER)
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#else
#include <strings.h>
#endif

std::string KnownHeaderMapper[]{"Accept",
                                "Accept-Charset",
                                "Accept-Encoding",
                                "Accept-Language",
                                "Authorization",
                                "Connection",
                                "Content-Length",
                                "Content-MD5",
                                "Content-Type",
                                "Date",
                                "Host",
                                "Server",
                                "User-Agent"};
} // namespace

HttpMessage::HttpMessage() : body_(nullptr), bodySize_(0), headers_() {}

HttpMessage::HttpMessage(const HttpMessage &other)
    : body_(nullptr), bodySize_(other.bodySize_), headers_(other.headers_) {
  setBody(other.body_, other.bodySize_);
}

HttpMessage::HttpMessage(HttpMessage &&other) { *this = std::move(other); }

HttpMessage &HttpMessage::operator=(const HttpMessage &other) {
  if (this != &other) {
    body_ = nullptr;
    bodySize_ = 0;
    headers_ = other.headers_;
    setBody(other.body_, other.bodySize_);
  }
  return *this;
}

HttpMessage &HttpMessage::operator=(HttpMessage &&other) {
  if (this != &other)
    *this = std::move(other);
  return *this;
}

void HttpMessage::addHeader(const HeaderNameType &name,
                            const HeaderValueType &value) {
  setHeader(name, value);
}

void HttpMessage::addHeader(KnownHeader header, const HeaderValueType &value) {
  setHeader(header, value);
}

HttpMessage::HeaderValueType
HttpMessage::header(const HeaderNameType &name) const {
  auto it = headers_.find(name);
  if (it != headers_.end())
    return it->second;
  else
    return std::string();
}

HttpMessage::HeaderCollection HttpMessage::headers() const { return headers_; }

void HttpMessage::removeHeader(const HeaderNameType &name) {
  headers_.erase(name);
}

void HttpMessage::removeHeader(KnownHeader header) {
  removeHeader(KnownHeaderMapper[header]);
}

void HttpMessage::setHeader(const HeaderNameType &name,
                            const HeaderValueType &value) {
  headers_[name] = value;
}

void HttpMessage::setHeader(KnownHeader header, const std::string &value) {
  setHeader(KnownHeaderMapper[header], value);
}

HttpMessage::~HttpMessage() { setBody(nullptr, 0); }

const char *HttpMessage::body() const { return body_; }

size_t HttpMessage::bodySize() const { return bodySize_; }

bool HttpMessage::hasBody() const { return (bodySize_ != 0); }

HttpMessage::HeaderValueType HttpMessage::header(KnownHeader header) const {
  return this->header(KnownHeaderMapper[header]);
}

void HttpMessage::setBody(const char *data, size_t size) {
  if (body_)
    delete[] body_;
  body_ = nullptr;
  bodySize_ = 0;
  if (size) {
    bodySize_ = size;
    body_ = new char[size + 1];
    std::copy(data, data + size, body_);
    body_[size] = '\0';
  }
}

bool HttpMessage::nocaseLess::operator()(const std::string &s1,
                                         const std::string &s2) const {
  return strcasecmp(s1.c_str(), s2.c_str()) < 0;
}

} // namespace AlibabaCloud
