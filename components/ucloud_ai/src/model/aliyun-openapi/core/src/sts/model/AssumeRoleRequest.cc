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

#include <alibabacloud/core/sts/model/AssumeRoleRequest.h>

// using namespace AlibabaCloud;
using namespace AlibabaCloud::Sts;
using namespace AlibabaCloud::Sts::Model;

AssumeRoleRequest::AssumeRoleRequest() : StsRequest("AssumeRole") {
  setRoleArn("");
  setRoleSessionName("");
  setDurationSeconds(3600);
}

AssumeRoleRequest::~AssumeRoleRequest() {}

int AssumeRoleRequest::durationSeconds() const {
  return std::stoi(parameter("DurationSeconds"));
}

std::string AssumeRoleRequest::policy() const { return parameter("Policy"); }

std::string AssumeRoleRequest::roleArn() const { return parameter("RoleArn"); }

std::string AssumeRoleRequest::roleSessionName() const {
  return parameter("RoleSessionName");
}

void AssumeRoleRequest::setDurationSeconds(int durationSeconds) {
  setParameter("DurationSeconds", std::to_string(durationSeconds));
}

void AssumeRoleRequest::setPolicy(const std::string &policy) {
  setParameter("Policy", policy);
}

void AssumeRoleRequest::setRoleArn(const std::string &roleArn) {
  setParameter("RoleArn", roleArn);
}

void AssumeRoleRequest::setRoleSessionName(const std::string &roleSessionName) {
  setParameter("RoleSessionName", roleSessionName);
}
