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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_HTTPCLIENT_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_HTTPCLIENT_H_

#include <alibabacloud/core/Error.h>
#include <alibabacloud/core/HttpRequest.h>
#include <alibabacloud/core/HttpResponse.h>
#include <alibabacloud/core/NetworkProxy.h>
#include <alibabacloud/core/Outcome.h>

namespace AlibabaCloud {
class HttpClient {
public:
  typedef Outcome<Error, HttpResponse> HttpResponseOutcome;

  HttpClient();
  virtual ~HttpClient();

  virtual HttpResponseOutcome makeRequest(const HttpRequest &request) = 0;
  NetworkProxy proxy() const;
  void setProxy(const NetworkProxy &proxy);
  bool rejectUnauthorized() const;
  void setRejectUnauthorized(const bool &rejectUnauthorized);

private:
  NetworkProxy proxy_;
  bool rejectUnauthorized_;
};
} // namespace AlibabaCloud
#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_HTTPCLIENT_H_
