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
#include <alibabacloud/core/ClientConfiguration.h>

namespace AlibabaCloud {

ClientConfiguration::ClientConfiguration(const std::string &regionId,
                                         const NetworkProxy &proxy)
    : regionId_(regionId), proxy_(proxy), endpoint_(),
      connectTimeout_(kDefaultConnectTimeout),
      readTimeout_(kDefaultReadTimeout) {}

ClientConfiguration::~ClientConfiguration() {}

std::string ClientConfiguration::endpoint() const { return endpoint_; }

NetworkProxy ClientConfiguration::proxy() const { return proxy_; }

std::string ClientConfiguration::regionId() const { return regionId_; }

void ClientConfiguration::setEndpoint(const std::string &endpoint) {
  endpoint_ = endpoint;
}

void ClientConfiguration::setProxy(const NetworkProxy &proxy) {
  proxy_ = proxy;
}

void ClientConfiguration::setRegionId(const std::string &regionId) {
  regionId_ = regionId;
}

long ClientConfiguration::connectTimeout() const { return connectTimeout_; }

long ClientConfiguration::readTimeout() const { return readTimeout_; }

void ClientConfiguration::setConnectTimeout(const long connectTimeout) {
  connectTimeout_ = connectTimeout;
}

void ClientConfiguration::setReadTimeout(const long readTimeout) {
  readTimeout_ = readTimeout;
}

bool ClientConfiguration::rejectUnauthorized() const {
  return rejectUnauthorized_;
}

void ClientConfiguration::setRejectUnauthorized(const bool rejectUnauthorized) {
  rejectUnauthorized_ = rejectUnauthorized;
}

} // namespace AlibabaCloud
