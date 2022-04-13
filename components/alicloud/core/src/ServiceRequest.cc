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

#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/core/ServiceRequest.h>
#include <alibabacloud/core/Utils.h>

namespace AlibabaCloud {

ServiceRequest::ServiceRequest(const std::string &product,
                               const std::string &version)
    : content_(nullptr), contentSize_(0), params_(), product_(product),
      resourcePath_("/"), version_(version), scheme_("https"),
      connectTimeout_(kInvalidTimeout), readTimeout_(kInvalidTimeout),
      method_(HttpRequest::Method::Get) {
  setHeader(std::string("x-acs-version"), version);
}

ServiceRequest::ServiceRequest(const ServiceRequest &other)
    : content_(nullptr), contentSize_(other.contentSize_),
      params_(other.params_), product_(other.product_),
      resourcePath_(other.resourcePath_), version_(other.version_),
      scheme_(other.scheme_), connectTimeout_(other.connectTimeout_),
      readTimeout_(other.readTimeout_) {
  setContent(other.content_, other.contentSize_);
}

ServiceRequest::ServiceRequest(ServiceRequest &&other) {
  *this = std::move(other);
}

ServiceRequest &ServiceRequest::operator=(const ServiceRequest &other) {
  if (this != &other) {
    content_ = nullptr;
    contentSize_ = 0;
    params_ = other.params_;
    connectTimeout_ = other.connectTimeout_;
    readTimeout_ = other.readTimeout_;
    setContent(other.content_, other.contentSize_);
  }
  return *this;
}

ServiceRequest &ServiceRequest::operator=(ServiceRequest &&other) {
  if (this != &other)
    *this = std::move(other);
  return *this;
}

ServiceRequest::~ServiceRequest() {
  if (content_)
    delete content_;
}

const char *ServiceRequest::content() const { return content_; }

size_t ServiceRequest::contentSize() const { return contentSize_; }

bool ServiceRequest::hasContent() const { return (contentSize_ != 0); }

void ServiceRequest::setContent(const char *data, size_t size) {
  if (content_)
    delete content_;
  content_ = nullptr;
  contentSize_ = 0;
  if (size) {
    contentSize_ = size;
    content_ = new char[size];
    std::copy(data, data + size, content_);
  }
}

void ServiceRequest::addParameter(const ParameterNameType &name,
                                  const ParameterValueType &value) {
  setParameter(name, value);
}

ServiceRequest::ParameterValueType
ServiceRequest::parameter(const ParameterNameType &name) const {
  ParameterCollection::const_iterator it = params_.find(name);
  if (it == params_.end()) {
    return ParameterValueType("");
  }
  return it->second;
}

ServiceRequest::ParameterValueType
ServiceRequest::coreParameter(const ParameterNameType &name) const {
  return parameter(name);
}

ServiceRequest::ParameterCollection ServiceRequest::parameters() const {
  return params_;
}

ServiceRequest::ParameterCollection ServiceRequest::bodyParameters() const {
  return body_params_;
}

void ServiceRequest::removeParameter(const ParameterNameType &name) {
  params_.erase(name);
}

void ServiceRequest::setParameter(const ParameterNameType &name,
                                  const ParameterValueType &value) {
  params_[name] = value;
}

void ServiceRequest::setCoreParameter(const ParameterNameType &name,
                                      const ParameterValueType &value) {
  setParameter(name, value);
}

void ServiceRequest::setBodyParameter(const ParameterNameType &name,
                                      const ParameterValueType &value) {
  body_params_[name] = value;
}

void ServiceRequest::setParameters(const ParameterCollection &params) {
  params_ = params;
}

void ServiceRequest::setJsonParameters(const ParameterNameType &name,
                                       const ParameterCollection &params) {
  params_ = params;
  params_ = params;
  setParameter(name, AlibabaCloud::MapToJson(params));
}

std::string ServiceRequest::version() const { return version_; }

HttpRequest::Method ServiceRequest::method() const { return method_; }

void ServiceRequest::setVersion(const std::string &version) {
  version_ = version;
}

std::string ServiceRequest::product() const { return product_; }

void ServiceRequest::setProduct(const std::string &product) {
  product_ = product;
}

std::string ServiceRequest::resourcePath() const { return resourcePath_; }

void ServiceRequest::setResourcePath(const std::string &path) {
  resourcePath_ = path;
}

void ServiceRequest::setScheme(const std::string scheme) { scheme_ = scheme; }

std::string ServiceRequest::scheme() const { return scheme_; }

long ServiceRequest::connectTimeout() const { return connectTimeout_; }

long ServiceRequest::readTimeout() const { return readTimeout_; }

void ServiceRequest::setConnectTimeout(const long connectTimeout) {
  connectTimeout_ = connectTimeout;
}

void ServiceRequest::setReadTimeout(const long readTimeout) {
  readTimeout_ = readTimeout;
}

void ServiceRequest::setMethod(const HttpRequest::Method method) {
  method_ = method;
}

void ServiceRequest::setHeader(
    const ServiceRequest::ParameterNameType &name,
    const ServiceRequest::ParameterValueType &value) {
  headers_[name] = value;
}

ServiceRequest::ParameterValueType
ServiceRequest::getHeader(const ServiceRequest::ParameterNameType &name) {
  return headers_[name];
}

void ServiceRequest::removeHeader(
    const ServiceRequest::ParameterNameType &name) {
  headers_.erase(name);
}

ServiceRequest::ParameterCollection ServiceRequest::headers() const {
  return headers_;
}

} // namespace AlibabaCloud
