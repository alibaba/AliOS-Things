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

#ifndef ALIBABACLOUD_CORE_STS_MODEL_GETCALLERIDENTITYRESULT_H_
#define ALIBABACLOUD_CORE_STS_MODEL_GETCALLERIDENTITYRESULT_H_

#include <alibabacloud/core/ServiceResult.h>

namespace AlibabaCloud {
namespace Sts {
namespace Model {
class ALIBABACLOUD_CORE_EXPORT GetCallerIdentityResult : public ServiceResult {
public:
  GetCallerIdentityResult();
  explicit GetCallerIdentityResult(const std::string &payload);
  ~GetCallerIdentityResult();

  std::string accountId();
  std::string arn() const;
  std::string userId() const;

private:
  void parse(const std::string &payload);

  std::string accountId_;
  std::string arn_;
  std::string userId_;
};
} // namespace Model
} // namespace Sts
} // namespace AlibabaCloud

#endif // !ALIBABACLOUD_CORE_STS_MODEL_GETCALLERIDENTITYRESULT_H_
