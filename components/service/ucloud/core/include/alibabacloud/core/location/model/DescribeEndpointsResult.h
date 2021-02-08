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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_LOCATION_MODEL_DESCRIBEENDPOINTSRESULT_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_LOCATION_MODEL_DESCRIBEENDPOINTSRESULT_H_

#include <alibabacloud/core/ServiceResult.h>
#include <string>
#include <utility>
#include <vector>

namespace AlibabaCloud {
namespace Location {
namespace Model {
class ALIBABACLOUD_CORE_EXPORT DescribeEndpointsResult : public ServiceResult {
public:
  struct Endpoint {
    std::string endpoint;
    std::string id;
    std::string namespace_;
    std::string serivceCode;
    std::string type;
    std::vector<std::string> protocols;
  };

  DescribeEndpointsResult();
  explicit DescribeEndpointsResult(const std::string &payload);
  ~DescribeEndpointsResult();

  std::vector<Endpoint> endpoints() const;
  void setEndpoints(const std::vector<Endpoint> &endpoints);

  bool success() const;
  void setSuccess(const bool &success);

protected:
  void parse(const std::string &payload);

private:
  std::vector<Endpoint> endpoints_;
  bool success_;
};
} // namespace Model
} // namespace Location
} // namespace AlibabaCloud
#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_LOCATION_MODEL_DESCRIBEENDPOINTSRESULT_H_
