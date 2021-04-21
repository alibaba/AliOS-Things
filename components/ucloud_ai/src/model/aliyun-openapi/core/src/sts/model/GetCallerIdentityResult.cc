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
#include <alibabacloud/core/sts/model/GetCallerIdentityResult.h>
#include <json/json.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Sts;
using namespace AlibabaCloud::Sts::Model;

GetCallerIdentityResult::GetCallerIdentityResult()
    : ServiceResult(), accountId_(), arn_(), userId_() {}

GetCallerIdentityResult::GetCallerIdentityResult(const std::string &payload)
    : ServiceResult(), accountId_(), arn_(), userId_() {
  parse(payload);
}

GetCallerIdentityResult::~GetCallerIdentityResult() {}

std::string GetCallerIdentityResult::accountId() { return accountId_; }

std::string GetCallerIdentityResult::arn() const { return arn_; }

std::string GetCallerIdentityResult::userId() const { return userId_; }

void GetCallerIdentityResult::parse(const std::string &payload) {
  Json::Reader reader;
  Json::Value value;
  reader.parse(payload, value);

  setRequestId(value["RequestId"].asString());

  accountId_ = value["AccountId"].asString();
  arn_ = value["Arn"].asString();
  userId_ = value["UserId"].asString();
}
