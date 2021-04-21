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

#ifndef CORE_SRC_ECSMETADATAFETCHER_H_
#define CORE_SRC_ECSMETADATAFETCHER_H_

#include <string>

namespace {
const int DEFAULT_TIMEOUT_IN_MILLISECONDS = 5000;
const char *const METADATA_SERVICE_HOST = "100.100.100.200";
const char *const URL_IN_ECS_METADATA =
    "/latest/meta-data/ram/security-credentials/";
} // namespace

namespace AlibabaCloud {
class EcsMetadataFetcher {
public:
  EcsMetadataFetcher();
  ~EcsMetadataFetcher();

  std::string roleName() const;
  void setRoleName(const std::string &roleName);
  std::string getMetadata(const std::string host, const std::string path);
  virtual std::string getMetadata();

private:
  std::string roleName_;
};
} // namespace AlibabaCloud

#endif // CORE_SRC_ECSMETADATAFETCHER_H_
