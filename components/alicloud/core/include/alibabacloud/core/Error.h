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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_ERROR_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_ERROR_H_

#include "CoreExport.h"
#include <string>

namespace AlibabaCloud {
class ALIBABACLOUD_CORE_EXPORT Error {
public:
  Error() = default;
  Error(std::string code, const std::string message);
  ~Error() = default;

  std::string errorCode() const;
  std::string errorMessage() const;
  std::string host() const;
  std::string requestId() const;
  std::string detail() const;
  void setErrorCode(const std::string &code);
  void setErrorMessage(const std::string &message);
  void setHost(const std::string &host);
  void setRequestId(const std::string &request);
  void setDetail(const std::string &detail);

private:
  std::string errorCode_;
  std::string message_;
  std::string host_;
  std::string requestId_;
  std::string detail_;
};
} // namespace AlibabaCloud

#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_ERROR_H_
