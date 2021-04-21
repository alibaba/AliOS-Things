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

#include "CurlHttpClient.h"
#include "Executor.h"
#include <alibabacloud/core/CoreClient.h>
#include <alibabacloud/core/Signer.h>
#include <alibabacloud/core/Utils.h>
#include <json/json.h>

/*!
 * \class AlibabaCloud::CoreClient CoreClient.h <alibabacloud/core/CoreClient.h>
 *
 */

namespace AlibabaCloud {

CoreClient::CoreClient(const std::string &servicename,
                       const ClientConfiguration &configuration)
    : serviceName_(servicename), configuration_(configuration),
      httpClient_(new CurlHttpClient) {
  httpClient_->setProxy(configuration.proxy());
  httpClient_->setRejectUnauthorized(configuration.rejectUnauthorized());
}

CoreClient::~CoreClient() { delete httpClient_; }

ClientConfiguration CoreClient::configuration() const { return configuration_; }

std::string CoreClient::serviceName() const { return serviceName_; }

void CoreClient::asyncExecute(Runnable *r) const {
  Executor::instance()->execute(r);
}

HttpClient::HttpResponseOutcome
CoreClient::AttemptRequest(const std::string &endpoint,
                           const ServiceRequest &request,
                           HttpRequest::Method method) const {
  auto r = buildHttpRequest(endpoint, request, method);
  auto outcome = httpClient_->makeRequest(r);
  if (!outcome.isSuccess())
    return outcome;
  if (hasResponseError(outcome.result()))
    return HttpClient::HttpResponseOutcome(buildCoreError(outcome.result()));
  else
    return outcome;
}

Error CoreClient::buildCoreError(const HttpResponse &response) const {
  Json::Reader reader;
  Json::Value value;
  if (!reader.parse(std::string(response.body(), response.bodySize()), value)) {
    if (response.bodySize() > 0) {
      return Error("InvalidResponse", response.body());
    } else {
      return Error("InvalidResponse", "body is empty");
    }
  }

  Error error;
  error.setErrorCode(value["Code"].asString());
  error.setErrorMessage(value["Message"].asString());
  error.setHost(value["HostId"].asString());
  error.setRequestId(value["RequestId"].asString());
  if (value["Code"].asString().empty() || value["Message"].asString().empty()) {
    error.setDetail(std::string(response.body()));
  }
  return error;
}

bool CoreClient::hasResponseError(const HttpResponse &response) const {
  return response.statusCode() < 200 || response.statusCode() > 299;
}

} // namespace AlibabaCloud
