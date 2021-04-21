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

#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/core/HttpRequest.h>

namespace AlibabaCloud {

HttpRequest::HttpRequest(const Url &url, Method method)
    : HttpMessage(), url_(url), method_(method),
      connectTimeout_(kDefaultConnectTimeout),
      readTimeout_(kDefaultReadTimeout) {}

HttpRequest::~HttpRequest() {}

HttpRequest::Method HttpRequest::method() const { return method_; }

void HttpRequest::setMethod(Method method) { method_ = method; }

void HttpRequest::setUrl(const Url &url) { url_ = url; }

Url HttpRequest::url() const { return url_; }

long HttpRequest::connectTimeout() const { return connectTimeout_; }

long HttpRequest::readTimeout() const { return readTimeout_; }

void HttpRequest::setConnectTimeout(const long connectTimeout) {
  connectTimeout_ = connectTimeout;
}

void HttpRequest::setReadTimeout(const long readTimeout) {
  readTimeout_ = readTimeout;
}

} // namespace AlibabaCloud
