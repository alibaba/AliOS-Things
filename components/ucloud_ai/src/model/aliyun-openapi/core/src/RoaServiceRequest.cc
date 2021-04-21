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

#include <algorithm>
#include <alibabacloud/core/RoaServiceRequest.h>
#include <alibabacloud/core/Utils.h>
#include <iomanip>
#include <sstream>

namespace AlibabaCloud {

RoaServiceRequest::RoaServiceRequest(const std::string &product,
                                     const std::string &version)
    : ServiceRequest(product, version) {}

RoaServiceRequest::~RoaServiceRequest() {}

} // namespace AlibabaCloud
