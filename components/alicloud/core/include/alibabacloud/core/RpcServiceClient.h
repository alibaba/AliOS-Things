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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_RPCSERVICECLIENT_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_RPCSERVICECLIENT_H_

#include "CoreClient.h"
#include "CredentialsProvider.h"
#include "HmacSha1Signer.h"
#include "HttpRequest.h"
#include "Outcome.h"
#include "RpcServiceRequest.h"
#include <map>
#include <memory>
#include <string>

namespace AlibabaCloud {
class RpcErrorMarshaller;
class RpcServiceRequest;
class ALIBABACLOUD_CORE_EXPORT RpcServiceClient : public CoreClient {
public:
  typedef Outcome<Error, std::string> JsonOutcome;

  RpcServiceClient(
      const std::string &servicename,
      const std::shared_ptr<CredentialsProvider> &credentialsProvider,
      const ClientConfiguration &configuration,
      const std::shared_ptr<Signer> &signer =
          std::make_shared<HmacSha1Signer>());
  virtual ~RpcServiceClient();

  std::shared_ptr<CredentialsProvider> credentialsProvider() {
    return credentialsProvider_;
  }
  using CoreClient::AttemptRequest;
  using CoreClient::configuration;

protected:
  virtual JsonOutcome
  makeRequest(const std::string &endpoint, const RpcServiceRequest &msg,
              HttpRequest::Method method = HttpRequest::Method::Get) const;
  HttpRequest buildHttpRequest(const std::string &endpoint,
                               const ServiceRequest &msg,
                               HttpRequest::Method method) const override;
  HttpRequest buildHttpRequest(const std::string &endpoint,
                               const RpcServiceRequest &msg,
                               HttpRequest::Method method) const;

private:
  std::shared_ptr<CredentialsProvider> credentialsProvider_;
  std::shared_ptr<Signer> signer_;
};
} // namespace AlibabaCloud
#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_RPCSERVICECLIENT_H_
