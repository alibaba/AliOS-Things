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

#include "EcsMetadataFetcher.h"
#include "CurlHttpClient.h"
#include <alibabacloud/core/Url.h>
#include <memory>
#include <sstream>

namespace AlibabaCloud {

EcsMetadataFetcher::EcsMetadataFetcher() {}

EcsMetadataFetcher::~EcsMetadataFetcher() {}

std::string EcsMetadataFetcher::roleName() const { return roleName_; }

void EcsMetadataFetcher::setRoleName(const std::string &roleName) {
  roleName_ = roleName;
}

std::string EcsMetadataFetcher::getMetadata() {
  return getMetadata(METADATA_SERVICE_HOST, URL_IN_ECS_METADATA);
}

std::string EcsMetadataFetcher::getMetadata(const std::string host,
                                            const std::string in_path) {
  std::stringstream path;
  path << in_path << roleName_;

  Url credentialUrl;
  credentialUrl.setHost(host);
  credentialUrl.setPath(path.str());

  auto client = std::make_shared<CurlHttpClient>();

  HttpRequest request;
  request.setUrl(credentialUrl);
  auto outcome = client->makeRequest(request);

  if (outcome.isSuccess())
    return std::string(outcome.result().body(), outcome.result().bodySize());
  else
    return outcome.error().errorCode();
}

} // namespace AlibabaCloud
