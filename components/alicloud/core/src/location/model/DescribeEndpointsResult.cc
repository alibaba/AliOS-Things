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

#include <alibabacloud/core/Utils.h>
#include <alibabacloud/core/location/model/DescribeEndpointsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Location::Model;

DescribeEndpointsResult::DescribeEndpointsResult() : ServiceResult() {}

DescribeEndpointsResult::DescribeEndpointsResult(const std::string &payload)
    : ServiceResult() {
  parse(payload);
}

DescribeEndpointsResult::~DescribeEndpointsResult() {}

void DescribeEndpointsResult::parse(const std::string &payload) {
  Json::Reader reader;
  Json::Value value;
  reader.parse(payload, value);

  setRequestId(value["RequestId"].asString());

  success_ = value["Success"].asBool();
  auto allEndpoints = value["Endpoints"]["Endpoint"];
  for (const auto &item : allEndpoints) {
    Endpoint region;
    region.endpoint = item["Endpoint"].asString();
    region.id = item["Id"].asString();
    region.namespace_ = item["Namespace"].asString();
    region.serivceCode = item["SerivceCode"].asString();
    region.type = item["Type"].asString();
    auto allProtocols = item["Protocols"]["Protocols"];
    for (const auto &item : allProtocols) {
      region.protocols.push_back(item.asString());
    }
    endpoints_.push_back(region);
  }
}

std::vector<DescribeEndpointsResult::Endpoint>
DescribeEndpointsResult::endpoints() const {
  return endpoints_;
}

void DescribeEndpointsResult::setEndpoints(
    const std::vector<Endpoint> &endpoints) {
  endpoints_ = endpoints;
}

bool DescribeEndpointsResult::success() const { return success_; }

void DescribeEndpointsResult::setSuccess(const bool &success) {
  success_ = success;
}
