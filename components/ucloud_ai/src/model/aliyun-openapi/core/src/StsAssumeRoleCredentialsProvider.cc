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

#include <alibabacloud/core/StsAssumeRoleCredentialsProvider.h>
#include <iomanip>
#include <sstream>

namespace AlibabaCloud {

StsAssumeRoleCredentialsProvider::StsAssumeRoleCredentialsProvider(
    const Credentials &credentials, const ClientConfiguration &configuration,
    const std::string &roleArn, const std::string &roleSessionName,
    const std::string &policy, int durationSeconds)
    : CredentialsProvider(), StsClient(credentials, configuration),
      roleArn_(roleArn), roleSessionName_(roleSessionName), policy_(policy),
      durationSeconds_(durationSeconds), cachedMutex_(),
      cachedCredentials_("", ""), expiry_() {}

StsAssumeRoleCredentialsProvider::~StsAssumeRoleCredentialsProvider() {}

Credentials StsAssumeRoleCredentialsProvider::getCredentials() {
  loadCredentials();
  std::lock_guard<std::mutex> locker(cachedMutex_);
  return cachedCredentials_;
}

bool StsAssumeRoleCredentialsProvider::checkExpiry() const {
  auto now = std::chrono::system_clock::now();
  auto diff =
      std::chrono::duration_cast<std::chrono::seconds>(now - expiry_).count();

  return (diff > 0 - 60);
}

void StsAssumeRoleCredentialsProvider::loadCredentials() {
  if (checkExpiry()) {
    std::lock_guard<std::mutex> locker(cachedMutex_);
    if (checkExpiry()) {
      Sts::Model::AssumeRoleRequest request;
      request.setRoleArn(roleArn_);
      request.setRoleSessionName(roleSessionName_);
      request.setPolicy(policy_);
      request.setDurationSeconds(durationSeconds_);

      auto assumeRoleOutcome = assumeRole(request);
      if (assumeRoleOutcome.isSuccess()) {
        const auto stsCredentials = assumeRoleOutcome.result().credentials();
        cachedCredentials_ = Credentials(stsCredentials.accessKeyId,
                                         stsCredentials.accessKeySecret,
                                         stsCredentials.securityToken);

        std::tm tm = {};
#if defined(__GNUG__) && __GNUC__ < 5
        strptime(stsCredentials.expiration.c_str(), "%Y-%m-%dT%H:%M:%SZ", &tm);
#else
        std::stringstream ss(stsCredentials.expiration);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
#endif
        expiry_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
      }
    }
  }
}

} // namespace AlibabaCloud
