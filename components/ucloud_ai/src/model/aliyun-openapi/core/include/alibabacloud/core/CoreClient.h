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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_CORECLIENT_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_CORECLIENT_H_

#include "ClientConfiguration.h"
#include "CoreExport.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Outcome.h"
#include "Runnable.h"
#include "ServiceRequest.h"
#include <functional>
#include <memory>
#include <string>

namespace AlibabaCloud {
class ALIBABACLOUD_CORE_EXPORT CoreClient {
public:
  CoreClient(const std::string &servicename,
             const ClientConfiguration &configuration);
  virtual ~CoreClient();

  ClientConfiguration configuration() const;
  std::string serviceName() const;

protected:
  virtual HttpClient::HttpResponseOutcome
  AttemptRequest(const std::string &endpoint, const ServiceRequest &request,
                 HttpRequest::Method method) const;
  Error buildCoreError(const HttpResponse &response) const;
  bool hasResponseError(const HttpResponse &response) const;
  virtual HttpRequest buildHttpRequest(const std::string &endpoint,
                                       const ServiceRequest &msg,
                                       HttpRequest::Method method) const = 0;
  void asyncExecute(Runnable *r) const;

private:
  std::string serviceName_;
  ClientConfiguration configuration_;
  std::shared_ptr<CredentialsProvider> credentialsProvider_;
  HttpClient *httpClient_;
};
} // namespace AlibabaCloud
#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_CORECLIENT_H_
