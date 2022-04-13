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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_CREDENTIALS_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_CREDENTIALS_H_

#include "CoreExport.h"
#include <string>

namespace AlibabaCloud {
class ALIBABACLOUD_CORE_EXPORT Credentials {
public:
  Credentials(const std::string &accessKeyId,
              const std::string &accessKeySecret,
              const std::string &sessionToken = "");
  ~Credentials();

  std::string accessKeyId() const;
  std::string accessKeySecret() const;
  void setAccessKeyId(const std::string &accessKeyId);
  void setAccessKeySecret(const std::string &accessKeySecret);
  void setSessionToken(const std::string &sessionToken);
  std::string sessionToken() const;

private:
  std::string accessKeyId_;
  std::string accessKeySecret_;
  std::string sessionToken_;
};
} // namespace AlibabaCloud

#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_CREDENTIALS_H_
