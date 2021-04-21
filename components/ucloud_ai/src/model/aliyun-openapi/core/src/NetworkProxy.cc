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

#include <alibabacloud/core/NetworkProxy.h>

namespace AlibabaCloud {

NetworkProxy::NetworkProxy(Type type, const std::string &hostName,
                           uint16_t port, const std::string &user,
                           const std::string &password)
    : hostName_(hostName), password_(password), port_(port), type_(type),
      user_(user) {}

NetworkProxy::~NetworkProxy() {}

std::string NetworkProxy::hostName() const { return hostName_; }

std::string NetworkProxy::password() const { return password_; }

uint16_t NetworkProxy::port() const { return port_; }

void NetworkProxy::setHostName(const std::string &hostName) {
  hostName_ = hostName;
}

void NetworkProxy::setPassword(const std::string &password) {
  password_ = password;
}

void NetworkProxy::setPort(uint16_t port) { port_ = port; }

void NetworkProxy::setType(NetworkProxy::Type type) { type_ = type; }

void NetworkProxy::setUser(const std::string &user) { user_ = user; }

NetworkProxy::Type NetworkProxy::type() const { return type_; }

std::string NetworkProxy::user() const { return user_; }

} // namespace AlibabaCloud
