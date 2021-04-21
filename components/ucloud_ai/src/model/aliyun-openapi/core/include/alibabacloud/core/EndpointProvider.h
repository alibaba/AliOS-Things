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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_ENDPOINTPROVIDER_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_ENDPOINTPROVIDER_H_

#include <alibabacloud/core/CoreClient.h>
#include <alibabacloud/core/location/LocationClient.h>
#include <chrono>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace AlibabaCloud {
class ALIBABACLOUD_CORE_EXPORT EndpointProvider
    : public Location::LocationClient {
public:
  typedef Outcome<Error, std::string> EndpointOutcome;

  EndpointProvider(
      const std::shared_ptr<Location::LocationClient> &locationClient,
      const std::string regionId, const std::string product,
      const std::string serviceCode = std::string(),
      int durationSeconds = 3600);

  EndpointProvider(const Credentials &credentials,
                   const ClientConfiguration &configuration,
                   const std::string &regionId, const std::string &product,
                   const std::string &serviceCode = std::string(),
                   int durationSeconds = 3600);
  ~EndpointProvider();

  EndpointOutcome getEndpoint();
  using LocationClient::describeEndpoints;

private:
  EndpointOutcome loadRemoteEndpoint();
  bool checkExpiry() const;

  bool loadLocalProductsInfo();
  std::string localEndpoint(const std::string regionId,
                            const std::string product);

  std::mutex cachedMutex_;
  std::string cachedEndpoint_;
  int durationSeconds_;
  std::chrono::system_clock::time_point expiry_;
  std::string regionId_;
  std::string product_;
  std::string serviceCode_;
};
} // namespace AlibabaCloud
#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_ENDPOINTPROVIDER_H_
