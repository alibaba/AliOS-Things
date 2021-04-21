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

#ifndef CORE_INCLUDE_ALIBABACLOUD_CORE_URL_H_
#define CORE_INCLUDE_ALIBABACLOUD_CORE_URL_H_

#include "CoreExport.h"
#include <string>

namespace AlibabaCloud {
class ALIBABACLOUD_CORE_EXPORT Url {
public:
  explicit Url(const std::string &url = "");
  Url(const Url &other) = default;
  Url(Url &&other) = default;
  ~Url();
  Url &operator=(const Url &url) = default;
  Url &operator=(Url &&other) = default;
  bool operator==(const Url &url) const;
  bool operator!=(const Url &url) const;

  std::string authority() const;
  void clear();
  std::string fragment() const;
  void fromString(const std::string &url);
  bool hasFragment() const;
  bool hasQuery() const;
  std::string host() const;
  bool isEmpty() const;
  bool isValid() const;
  int port() const;
  std::string password() const;
  std::string path() const;
  std::string query() const;
  std::string scheme() const;
  void setAuthority(const std::string &authority);
  void setFragment(const std::string &fragment);
  void setHost(const std::string &host);
  void setPassword(const std::string &password);
  void setPath(const std::string &path);
  void setPort(int port);
  void setQuery(const std::string &query);
  void setScheme(const std::string &scheme);
  void setUserInfo(const std::string &userInfo);
  void setUserName(const std::string &userName);
  std::string toString() const;
  std::string userInfo() const;
  std::string userName() const;

private:
  std::string scheme_;
  std::string userName_;
  std::string password_;
  std::string host_;
  std::string path_;
  int port_;
  std::string query_;
  std::string fragment_;
};
} // namespace AlibabaCloud
#endif // CORE_INCLUDE_ALIBABACLOUD_CORE_URL_H_
