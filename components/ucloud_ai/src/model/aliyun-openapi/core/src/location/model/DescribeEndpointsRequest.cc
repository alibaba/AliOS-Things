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

#include <alibabacloud/core/location/model/DescribeEndpointsRequest.h>

using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Location::Model;

DescribeEndpointsRequest::DescribeEndpointsRequest()
    : LocationRequest("DescribeEndpoints") {}

DescribeEndpointsRequest::~DescribeEndpointsRequest() {}

std::string DescribeEndpointsRequest::serviceCode() const {
  return parameter("ServiceCode");
}

void DescribeEndpointsRequest::setServiceCode(const std::string &serviceCode) {
  setParameter("ServiceCode", serviceCode);
}

std::string DescribeEndpointsRequest::id() const { return parameter("Id"); }

void DescribeEndpointsRequest::setId(const std::string &id) {
  setParameter("Id", id);
}

std::string DescribeEndpointsRequest::type() const { return parameter("Type"); }

void DescribeEndpointsRequest::setType(const std::string &type) {
  setParameter("Type", type);
}
