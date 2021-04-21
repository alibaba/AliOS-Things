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

#include <alibabacloud/core/Credentials.h>

namespace AlibabaCloud {

Credentials::Credentials(const std::string &accessKeyId,
                         const std::string &accessKeySecret,
                         const std::string &sessionToken)
    : accessKeyId_(accessKeyId), accessKeySecret_(accessKeySecret),
      sessionToken_(sessionToken) {}

Credentials::~Credentials() {}

std::string Credentials::accessKeyId() const { return accessKeyId_; }

std::string Credentials::accessKeySecret() const { return accessKeySecret_; }

void Credentials::setAccessKeyId(const std::string &accessKeyId) {
  accessKeyId_ = accessKeyId;
}

void Credentials::setAccessKeySecret(const std::string &accessKeySecret) {
  accessKeySecret_ = accessKeySecret;
}

void Credentials::setSessionToken(const std::string &sessionToken) {
  sessionToken_ = sessionToken;
}

std::string Credentials::sessionToken() const { return sessionToken_; }

} // namespace AlibabaCloud
