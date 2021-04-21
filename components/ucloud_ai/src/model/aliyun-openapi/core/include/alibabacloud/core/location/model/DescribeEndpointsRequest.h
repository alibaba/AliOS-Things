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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_LOCATION_MODEL_DESCRIBEENDPOINTSREQUEST_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_LOCATION_MODEL_DESCRIBEENDPOINTSREQUEST_H_

#include <alibabacloud/core/location/LocationRequest.h>
#include <string>

namespace AlibabaCloud {
namespace Location {
namespace Model {
class ALIBABACLOUD_CORE_EXPORT DescribeEndpointsRequest
    : public LocationRequest {
public:
  DescribeEndpointsRequest();
  ~DescribeEndpointsRequest();

  std::string serviceCode() const;
  void setServiceCode(const std::string &serviceCode);
  std::string id() const;
  void setId(const std::string &id);
  std::string type() const;
  void setType(const std::string &type);
};
} // namespace Model
} // namespace Location
} // namespace AlibabaCloud
#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_LOCATION_MODEL_DESCRIBEENDPOINTSREQUEST_H_
