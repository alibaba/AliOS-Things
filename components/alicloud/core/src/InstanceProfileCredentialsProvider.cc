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

#include "EcsMetadataFetcher.h"
#include <alibabacloud/core/InstanceProfileCredentialsProvider.h>
#include <alibabacloud/core/Utils.h>
#include <chrono>
#include <iomanip>
#include <json/json.h>
#include <mutex>
#include <sstream>

namespace AlibabaCloud {

InstanceProfileCredentialsProvider::InstanceProfileCredentialsProvider(
    const std::string &roleName, int durationSeconds)
    : CredentialsProvider(), EcsMetadataFetcher(),
      durationSeconds_(durationSeconds), cachedMutex_(),
      cachedCredentials_("", ""), expiry_() {
  setRoleName(roleName);
}

InstanceProfileCredentialsProvider::~InstanceProfileCredentialsProvider() {}

Credentials InstanceProfileCredentialsProvider::getCredentials() {
  loadCredentials();
  std::lock_guard<std::mutex> locker(cachedMutex_);
  return cachedCredentials_;
}

bool InstanceProfileCredentialsProvider::checkExpiry() const {
  auto now = std::chrono::system_clock::now();
  auto diff =
      std::chrono::duration_cast<std::chrono::seconds>(now - expiry_).count();

  return (diff > 0 - 60);
}

void InstanceProfileCredentialsProvider::loadCredentials() {
  if (checkExpiry()) {
    std::lock_guard<std::mutex> locker(cachedMutex_);
    if (checkExpiry()) {
      auto outcome = getMetadata();
      Json::Value value;
      Json::Reader reader;
      if (reader.parse(outcome, value)) {
        if (value["Code"].empty() && value["AccessKeyId"].empty() &&
            value["AccessKeySecret"].empty() &&
            value["SecurityToken"].empty() && value["Expiration"].empty()) {
          cachedCredentials_ = Credentials("", "");
          return;
        }

        auto code = value["Code"].asString();
        auto accessKeyId = value["AccessKeyId"].asString();
        auto accessKeySecret = value["AccessKeySecret"].asString();
        auto securityToken = value["SecurityToken"].asString();
        auto expiration = value["Expiration"].asString();

        cachedCredentials_ =
            Credentials(accessKeyId, accessKeySecret, securityToken);

        std::tm tm = {};
#if defined(__GNUG__) && __GNUC__ < 5
        strptime(expiration.c_str(), "%Y-%m-%dT%H:%M:%SZ", &tm);
#else
        std::stringstream ss(expiration);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
#endif
        expiry_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
      }
    }
  }
}

} // namespace AlibabaCloud
