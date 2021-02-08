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
#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/core/HmacSha1Signer.h>
#include <alibabacloud/core/RoaServiceClient.h>
#include <alibabacloud/core/Utils.h>
#include <iomanip>
#include <sstream>
#ifdef USE_AOS_TIME_POSIX_API
#include <posix/timer.h>
#endif

namespace AlibabaCloud {

RoaServiceClient::RoaServiceClient(
    const std::string &servicename,
    const std::shared_ptr<CredentialsProvider> &credentialsProvider,
    const ClientConfiguration &configuration,
    const std::shared_ptr<Signer> &signer)
    : CoreClient(servicename, configuration),
      credentialsProvider_(credentialsProvider), signer_(signer) {}

RoaServiceClient::~RoaServiceClient() {}

RoaServiceClient::JsonOutcome
RoaServiceClient::makeRequest(const std::string &endpoint,
                              const RoaServiceRequest &request,
                              HttpRequest::Method method) const {
  if (method == HttpRequest::Method::Get) {
    method = request.method();
  }
  auto outcome = AttemptRequest(endpoint, request, method);
  if (outcome.isSuccess())
    return JsonOutcome(
        std::string(outcome.result().body(), outcome.result().bodySize()));
  else
    return JsonOutcome(outcome.error());
}

HttpRequest
RoaServiceClient::buildHttpRequest(const std::string &endpoint,
                                   const ServiceRequest &msg,
                                   HttpRequest::Method method) const {
  return buildHttpRequest(endpoint,
                          static_cast<const RoaServiceRequest &>(msg), method); //lk change dynamic_cast to static_cast
}

HttpRequest
RoaServiceClient::buildHttpRequest(const std::string &endpoint,
                                   const RoaServiceRequest &msg,
                                   HttpRequest::Method method) const {
  const Credentials credentials = credentialsProvider_->getCredentials();

  Url url;
  if (msg.scheme().empty()) {
    url.setScheme("https");
  } else {
    url.setScheme(msg.scheme());
  }
  url.setHost(endpoint);
  url.setPath(msg.resourcePath());

  auto params = msg.parameters();
  std::map<std::string, std::string> queryParams;
  for (const auto &p : params) {
    if (!p.second.empty())
      queryParams[p.first] = p.second;
  }

  if (!queryParams.empty()) {
    std::stringstream queryString;
    for (const auto &p : queryParams) {
      if (p.second.empty())
        queryString << "&" << p.first;
      else
        queryString << "&" << p.first << "=" << p.second;
    }
    url.setQuery(queryString.str().substr(1));
  }

  HttpRequest request(url);
  request.setMethod(method);
  if (msg.connectTimeout() != kInvalidTimeout) {
    request.setConnectTimeout(msg.connectTimeout());
  } else {
    request.setConnectTimeout(configuration().connectTimeout());
  }

  if (msg.readTimeout() != kInvalidTimeout) {
    request.setReadTimeout(msg.readTimeout());
  } else {
    request.setReadTimeout(configuration().readTimeout());
  }

  for (const auto &h : msg.headers()) {
    if (!h.second.empty()) {
      request.setHeader(h.first, h.second);
    }
  }

  if (msg.parameter("Accept").empty()) {
    request.setHeader("Accept", "application/json");
  } else {
    request.setHeader("Accept", msg.parameter("Accept"));
  }

  if (msg.hasContent()) {
    std::stringstream ss;
    ss << msg.contentSize();
    request.setHeader("Content-Length", ss.str());
    if (msg.parameter("Content-Type").empty()) {
      request.setHeader("Content-Type", "application/octet-stream");
    } else {
      request.setHeader("Content-Type", msg.parameter("Content-Type"));
    }
    request.setHeader("Content-MD5",
                      ComputeContentMD5(msg.content(), msg.contentSize()));
    request.setBody(msg.content(), msg.contentSize());
  } else {
    if (msg.parameter("Content-Type").empty()) {
      request.setHeader("Content-Type", "application/x-www-form-urlencoded");
      auto body_params = msg.bodyParameters();
      std::stringstream sbody;
      for (const auto &p : body_params) {
        sbody << "&" << p.first << "=" << UrlEncode(p.second);
      }
      request.setBody(sbody.str().c_str(), sbody.str().size());
      request.setHeader("Content-Length", std::to_string(request.bodySize()));
      request.setHeader("Content-MD5",
                        ComputeContentMD5(request.body(), request.bodySize()));
    } else {
      request.setHeader("Content-MD5",
                        ComputeContentMD5(msg.content(), msg.contentSize()));
    }
  }

#ifdef USE_AOS_TIME_POSIX_API
    struct timespec currentTime;
    time_t t;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    t = currentTime.tv_nsec/1000000000 + currentTime.tv_sec;
#else
    std::time_t t = std::time(nullptr);
#endif
  std::stringstream date;
#if defined(__GNUG__) && __GNUC__ < 5
  char tmbuff[26];
  strftime(tmbuff, 26, "%a, %d %b %Y %T", std::gmtime(&t));
  date << tmbuff << " GMT";
#else
  date << std::put_time(std::gmtime(&t), "%a, %d %b %Y %T GMT");
#endif
  request.setHeader("Date", date.str());
  request.setHeader("Host", url.host());
  request.setHeader("x-sdk-client",
                    std::string("CPP/").append(ALIBABACLOUD_VERSION_STR));
  request.setHeader("x-acs-region-id", configuration().regionId());
  if (!credentials.sessionToken().empty())
    request.setHeader("x-acs-security-token", credentials.sessionToken());
  request.setHeader("x-acs-signature-method", signer_->name());
  request.setHeader("x-acs-signature-nonce", GenerateUuid());
  request.setHeader("x-acs-signature-version", signer_->version());
  request.setHeader("x-acs-version", msg.version());

  std::stringstream plaintext;
  plaintext << HttpMethodToString(method) << "\n"
            << request.header("Accept") << "\n"
            << request.header("Content-MD5") << "\n"
            << request.header("Content-Type") << "\n"
            << request.header("Date") << "\n"
            << canonicalizedHeaders(request.headers());
  if (!url.hasQuery())
    plaintext << url.path();
  else
    plaintext << url.path() << "?" << url.query();

  std::stringstream sign;
  sign << "acs " << credentials.accessKeyId() << ":"
       << signer_->generate(plaintext.str(), credentials.accessKeySecret());
  request.setHeader("Authorization", sign.str());
  return request;
}

} // namespace AlibabaCloud
