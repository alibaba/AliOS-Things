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

#include <algorithm>
#include <alibabacloud/core/Url.h>
#include <sstream>

namespace AlibabaCloud {

namespace {
#define INVALID_PORT -1
} // namespace

Url::Url(const std::string &url)
    : scheme_(), userName_(), password_(), host_(), path_(),
      port_(INVALID_PORT), query_(), fragment_() {
  if (!url.empty())
    fromString(url);
}

Url::~Url() {}

bool Url::operator==(const Url &url) const {
  return scheme_ == url.scheme_ && userName_ == url.userName_ &&
         password_ == url.password_ && host_ == url.host_ &&
         path_ == url.path_ && port_ == url.port_ && query_ == url.query_ &&
         fragment_ == url.fragment_;
}

bool Url::operator!=(const Url &url) const { return !(*this == url); }

std::string Url::authority() const {
  if (!isValid())
    return std::string();

  std::ostringstream out;
  std::string str = userInfo();
  if (!str.empty())
    out << str << "@";
  out << host_;
  if (port_ != INVALID_PORT)
    out << ":" << port_;
  return out.str();
}

void Url::clear() {
  scheme_.clear();
  userName_.clear();
  password_.clear();
  host_.clear();
  path_.clear();
  port_ = INVALID_PORT;
  query_.clear();
  fragment_.clear();
}

std::string Url::fragment() const { return fragment_; }

void Url::fromString(const std::string &url) {
  clear();
  if (url.empty())
    return;

  std::string str = url;
  std::string::size_type pos = 0;
  std::string authority, fragment, path, query, scheme;

  pos = str.find("://");
  if (pos != str.npos) {
    scheme = str.substr(0, pos);
    str.erase(0, pos + 3);
  }

  pos = str.find('#');
  if (pos != str.npos) {
    fragment = str.substr(pos + 1);
    str.erase(pos);
  }

  pos = str.find('?');
  if (pos != str.npos) {
    query = str.substr(pos + 1);
    str.erase(pos);
  }

  pos = str.find('/');
  if (pos != str.npos) {
    path = str.substr(pos);
    str.erase(pos);
  } else {
    path = "/";
  }

  authority = str;

  setScheme(scheme);
  setAuthority(authority);
  setPath(path);
  setQuery(query);
  setFragment(fragment);
}

bool Url::hasFragment() const { return !fragment_.empty(); }

bool Url::hasQuery() const { return !query_.empty(); }

std::string Url::host() const { return host_; }

bool Url::isEmpty() const {
  return scheme_.empty() && userName_.empty() && password_.empty() &&
         host_.empty() && path_.empty() && (port_ == INVALID_PORT) &&
         query_.empty() && fragment_.empty();
}

bool Url::isValid() const {
  if (isEmpty())
    return false;

  if (host_.empty())
    return false;

  bool valid = true;
  if (userName_.empty())
    valid = password_.empty();
  return valid;
}

int Url::port() const { return port_; }

std::string Url::password() const { return password_; }

std::string Url::path() const { return path_; }

std::string Url::query() const { return query_; }

std::string Url::scheme() const { return scheme_; }

void Url::setAuthority(const std::string &authority) {
  if (authority.empty()) {
    setUserInfo("");
    setHost("");
    setPort(INVALID_PORT);
    return;
  }

  std::string userinfo, host, port;
  std::string::size_type pos = 0, prevpos = 0;

  pos = authority.find('@');
  if (pos != authority.npos) {
    userinfo = authority.substr(0, pos);
    prevpos = pos + 1;
  } else {
    pos = 0;
  }

  pos = authority.find(':', prevpos);
  if (pos == authority.npos) {
    host = authority.substr(prevpos);
  } else {
    host = authority.substr(prevpos, pos - prevpos);
    port = authority.substr(pos + 1);
  }

  setUserInfo(userinfo);
  setHost(host);
  setPort(!port.empty() ? atoi(port.c_str()) : INVALID_PORT);
}

void Url::setFragment(const std::string &fragment) { fragment_ = fragment; }

void Url::setHost(const std::string &host) {
  if (host.empty()) {
    host_.clear();
    return;
  }
  host_ = host;
  std::transform(host_.begin(), host_.end(), host_.begin(), ::tolower);
}

void Url::setPassword(const std::string &password) { password_ = password; }

void Url::setPath(const std::string &path) { path_ = path; }

void Url::setPort(int port) { port_ = port; }

void Url::setQuery(const std::string &query) { query_ = query; }

void Url::setScheme(const std::string &scheme) {
  if (scheme.empty()) {
    scheme_.clear();
    return;
  }
  scheme_ = scheme;
  std::transform(scheme_.begin(), scheme_.end(), scheme_.begin(), ::tolower);
}

void Url::setUserInfo(const std::string &userInfo) {
  if (userInfo.empty()) {
    userName_.clear();
    password_.clear();
    return;
  }

  auto pos = userInfo.find(':');
  if (pos == userInfo.npos) {
    userName_ = userInfo;
  } else {
    userName_ = userInfo.substr(0, pos);
    password_ = userInfo.substr(pos + 1);
  }
}

void Url::setUserName(const std::string &userName) { userName_ = userName; }

std::string Url::toString() const {
  if (!isValid())
    return std::string();

  std::ostringstream out;
  if (!scheme_.empty())
    out << scheme_ << "://";
  std::string str = authority();
  if (!str.empty())
    out << authority();
  if (path_.empty())
    out << "/";
  else
    out << path_;
  if (hasQuery())
    out << "?" << query_;
  if (hasFragment())
    out << "#" << fragment_;
  return out.str();
}

std::string Url::userInfo() const {
  if (!isValid())
    return std::string();

  std::ostringstream out;
  out << userName_;
  if (!password_.empty())
    out << ":" << password_;
  return out.str();
}

std::string Url::userName() const { return userName_; }

} // namespace AlibabaCloud
