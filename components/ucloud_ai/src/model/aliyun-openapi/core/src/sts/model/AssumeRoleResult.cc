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

#include <alibabacloud/core/Utils.h>
#include <alibabacloud/core/sts/model/AssumeRoleResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Sts;
using namespace AlibabaCloud::Sts::Model;

AssumeRoleResult::AssumeRoleResult()
    : ServiceResult(), assumedRoleUser_(), credentials_() {}

AssumeRoleResult::AssumeRoleResult(const std::string &payload)
    : ServiceResult(), assumedRoleUser_(), credentials_() {
  parse(payload);
}

AssumeRoleResult::~AssumeRoleResult() {}

AssumeRoleResult::AssumedRoleUser AssumeRoleResult::assumedRoleUser() const {
  return assumedRoleUser_;
}

AssumeRoleResult::Credentials AssumeRoleResult::credentials() const {
  return credentials_;
}

void AssumeRoleResult::parse(const std::string &payload) {
  Json::Reader reader;
  Json::Value value;
  reader.parse(payload, value);

  setRequestId(value["RequestId"].asString());

  auto assumedRoleUserNode = value["AssumedRoleUser"];
  assumedRoleUser_.assumedRoleId =
      assumedRoleUserNode["AssumedRoleId"].asString();
  assumedRoleUser_.arn = assumedRoleUserNode["Arn"].asString();

  auto credentialsNode = value["Credentials"];
  credentials_.accessKeyId = credentialsNode["AccessKeyId"].asString();
  credentials_.accessKeySecret = credentialsNode["AccessKeySecret"].asString();
  credentials_.expiration = credentialsNode["Expiration"].asString();
  credentials_.securityToken = credentialsNode["SecurityToken"].asString();
}
