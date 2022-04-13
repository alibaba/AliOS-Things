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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_HTTPREQUEST_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_HTTPREQUEST_H_

#include "HttpMessage.h"
#include "Url.h"
#include <string>

namespace AlibabaCloud {
class ALIBABACLOUD_CORE_EXPORT HttpRequest : public HttpMessage {
public:
  enum Method { Get, Head, Post, Put, Delete, Connect, Options, Patch, Trace };
  explicit HttpRequest(const Url &url = Url(), Method method = Get);
  ~HttpRequest();

  Method method() const;
  void setMethod(Method method);
  void setUrl(const Url &url);
  Url url() const;
  long connectTimeout() const;
  long readTimeout() const;
  void setConnectTimeout(const long connectTimeout);
  void setReadTimeout(const long readTimeout);

private:
  Method method_;
  Url url_;
  long connectTimeout_;
  long readTimeout_;
};
} // namespace AlibabaCloud
#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_HTTPREQUEST_H_
