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

#include <alibabacloud/core/HttpClient.h>
#include <cassert>
#include <sstream>
#include <vector>

namespace AlibabaCloud {

HttpClient::HttpClient() : proxy_() {}

HttpClient::~HttpClient() {}

NetworkProxy HttpClient::proxy() const { return proxy_; }

void HttpClient::setProxy(const NetworkProxy &proxy) { proxy_ = proxy; }

bool HttpClient::rejectUnauthorized() const { return rejectUnauthorized_; }

void HttpClient::setRejectUnauthorized(const bool &rejectUnauthorized) {
  rejectUnauthorized_ = rejectUnauthorized;
}

} // namespace AlibabaCloud
