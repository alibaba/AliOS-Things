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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_COMMONCLIENT_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_COMMONCLIENT_H_

#include "AsyncCallerContext.h"
#include "ClientConfiguration.h"
#include "CommonRequest.h"
#include "CommonResponse.h"
#include "CoreClient.h"
#include "CoreExport.h"
#include "CredentialsProvider.h"
#include <future>
#include <memory>
#include <string>

namespace AlibabaCloud {
class ALIBABACLOUD_CORE_EXPORT CommonClient : public CoreClient {
public:
  typedef Outcome<Error, CommonResponse> CommonResponseOutcome;
  typedef std::future<CommonResponseOutcome> CommonResponseOutcomeCallable;
  typedef std::function<void(const CommonClient *, const CommonRequest &,
                             const CommonResponseOutcome &,
                             const std::shared_ptr<const AsyncCallerContext> &)>
      CommonResponseAsyncHandler;
  typedef Outcome<Error, std::string> JsonOutcome;

  CommonClient(const Credentials &credentials,
               const ClientConfiguration &configuration);
  CommonClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider,
               const ClientConfiguration &configuration);
  CommonClient(const std::string &accessKeyId,
               const std::string &accessKeySecret,
               const ClientConfiguration &configuration);
  ~CommonClient();

  CommonResponseOutcome commonResponse(const CommonRequest &request) const;
  void commonResponseAsync(
      const CommonRequest &request, const CommonResponseAsyncHandler &handler,
      const std::shared_ptr<const AsyncCallerContext> &context = nullptr) const;
  CommonResponseOutcomeCallable
  commonResponseCallable(const CommonRequest &request) const;

protected:
  HttpRequest buildHttpRequest(const std::string &endpoint,
                               const ServiceRequest &msg,
                               HttpRequest::Method method) const override;
  HttpRequest buildHttpRequest(const std::string &endpoint,
                               const CommonRequest &msg,
                               HttpRequest::Method method) const;
  HttpRequest buildRoaHttpRequest(const std::string &endpoint,
                                  const CommonRequest &msg,
                                  HttpRequest::Method method) const;
  HttpRequest buildRpcHttpRequest(const std::string &endpoint,
                                  const CommonRequest &msg,
                                  HttpRequest::Method method) const;
  JsonOutcome
  makeRequest(const std::string &endpoint, const CommonRequest &msg,
              HttpRequest::Method method = HttpRequest::Method::Get) const;
  using CoreClient::asyncExecute;

private:
  std::shared_ptr<CredentialsProvider> credentialsProvider_;
  std::shared_ptr<Signer> signer_;
};
} // namespace AlibabaCloud
#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_COMMONCLIENT_H_
