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

#ifndef ALIBABACLOUD_CORE_STS_MODEL_ASSUMEROLEREQUEST_H_
#define ALIBABACLOUD_CORE_STS_MODEL_ASSUMEROLEREQUEST_H_

#include <alibabacloud/core/sts/StsRequest.h>
#include <string>

namespace AlibabaCloud {
namespace Sts {
namespace Model {
class ALIBABACLOUD_CORE_EXPORT AssumeRoleRequest : public StsRequest {
public:
  AssumeRoleRequest();
  ~AssumeRoleRequest();

  int durationSeconds() const;
  std::string policy() const;
  std::string roleArn() const;
  std::string roleSessionName() const;
  void setDurationSeconds(int durationSeconds);
  void setPolicy(const std::string &policy);
  void setRoleArn(const std::string &roleArn);
  void setRoleSessionName(const std::string &roleSessionName);
};
} // namespace Model
} // namespace Sts
} // namespace AlibabaCloud
#endif // !ALIBABACLOUD_CORE_STS_MODEL_ASSUMEROLEREQUEST_H_
