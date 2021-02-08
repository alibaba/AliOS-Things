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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_CLIENTCONFIGURATION_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_CLIENTCONFIGURATION_H_

#include "CoreExport.h"
#include "CredentialsProvider.h"
#include "NetworkProxy.h"
#include "Signer.h"
#include <memory>
#include <string>

namespace AlibabaCloud {
class ALIBABACLOUD_CORE_EXPORT ClientConfiguration {
public:
  explicit ClientConfiguration(const std::string &regionId = "cn-hangzhou",
                               const NetworkProxy &proxy = NetworkProxy());
  ~ClientConfiguration();

  std::string endpoint() const;
  NetworkProxy proxy() const;
  std::string regionId() const;
  void setEndpoint(const std::string &endpoint);
  void setProxy(const NetworkProxy &proxy);
  void setRegionId(const std::string &regionId);

  long connectTimeout() const;
  long readTimeout() const;
  void setConnectTimeout(const long connectTimeout);
  void setReadTimeout(const long readTimeout);

  bool rejectUnauthorized() const;
  void setRejectUnauthorized(const bool rejectUnauthorized);

private:
  std::string endpoint_;
  NetworkProxy proxy_;
  std::string regionId_;
  long connectTimeout_;
  long readTimeout_;
  bool rejectUnauthorized_ = true;
};
} // namespace AlibabaCloud

#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_CLIENTCONFIGURATION_H_
