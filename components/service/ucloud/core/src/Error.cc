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

#include <alibabacloud/core/Error.h>

namespace AlibabaCloud {

Error::Error(std::string code, const std::string message)
    : errorCode_(code), message_(message), host_(), requestId_(), detail_() {}

std::string Error::errorCode() const { return errorCode_; }
std::string Error::errorMessage() const { return message_; }
std::string Error::host() const { return host_; }
std::string Error::requestId() const { return requestId_; }
std::string Error::detail() const { return detail_; }
void Error::setErrorCode(const std::string &code) { errorCode_ = code; }
void Error::setErrorMessage(const std::string &message) { message_ = message; }
void Error::setHost(const std::string &host) { host_ = host; }
void Error::setRequestId(const std::string &request) { requestId_ = request; }
void Error::setDetail(const std::string &detail) { detail_ = detail; }

} // namespace AlibabaCloud
