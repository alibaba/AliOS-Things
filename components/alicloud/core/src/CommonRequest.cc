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

#include <alibabacloud/core/CommonRequest.h>

namespace AlibabaCloud {

CommonRequest::CommonRequest(RequestPattern pattern)
    : ServiceRequest("", ""), domain_(), queryParams_(),
      httpMethod_(HttpRequest::Get), requestPattern_(pattern) {}

CommonRequest::~CommonRequest() {}

std::string CommonRequest::domain() const { return domain_; }

void CommonRequest::setDomain(const std::string &domain) { domain_ = domain; }

CommonRequest::RequestPattern CommonRequest::requestPattern() const {
  return requestPattern_;
}

void CommonRequest::setRequestPattern(RequestPattern pattern) {
  requestPattern_ = pattern;
}

void CommonRequest::setHttpMethod(HttpRequest::Method method) {
  httpMethod_ = method;
}

HttpRequest::Method CommonRequest::httpMethod() const { return httpMethod_; }

CommonRequest::ParameterValueType
CommonRequest::queryParameter(const ParameterNameType &name) const {
  ParameterCollection::const_iterator it = queryParams_.find(name);
  if (it == queryParams_.end()) {
    return ParameterValueType("");
  }
  return it->second;
}

CommonRequest::ParameterCollection CommonRequest::queryParameters() const {
  return queryParams_;
}

void CommonRequest::setQueryParameter(const ParameterNameType &name,
                                      const ParameterValueType &value) {
  queryParams_[name] = value;
}

CommonRequest::ParameterValueType
CommonRequest::headerParameter(const ParameterNameType &name) const {
  const ParameterCollection::const_iterator it = headerParams_.find(name);
  if (it == headerParams_.end()) {
    return ParameterValueType("");
  }
  return it->second;
}

CommonRequest::ParameterCollection CommonRequest::headerParameters() const {
  return headerParams_;
}

void CommonRequest::setHeaderParameter(const ParameterNameType &name,
                                       const ParameterValueType &value) {
  headerParams_[name] = value;
}

} // namespace AlibabaCloud
