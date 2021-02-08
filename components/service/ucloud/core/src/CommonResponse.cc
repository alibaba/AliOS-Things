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

#include <alibabacloud/core/CommonResponse.h>

namespace AlibabaCloud {

CommonResponse::CommonResponse() : payload_() {}

CommonResponse::CommonResponse(const std::string &payload)
    : payload_(payload) {}

CommonResponse::~CommonResponse() {}

std::string CommonResponse::payload() const { return payload_; }

} // namespace AlibabaCloud
