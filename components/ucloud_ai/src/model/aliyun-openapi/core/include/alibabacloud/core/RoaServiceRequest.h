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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_ROASERVICEREQUEST_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_ROASERVICEREQUEST_H_

#include "ServiceRequest.h"
#include <string>

namespace AlibabaCloud {
class ALIBABACLOUD_CORE_EXPORT RoaServiceRequest : public ServiceRequest {
public:
  RoaServiceRequest(const std::string &product, const std::string &version);
  virtual ~RoaServiceRequest();
  using ServiceRequest::parameter;
  using ServiceRequest::setContent;
  using ServiceRequest::setMethod;
  using ServiceRequest::setParameter;
  using ServiceRequest::setResourcePath;
  using ServiceRequest::setScheme;
};
} // namespace AlibabaCloud
#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_ROASERVICEREQUEST_H_
