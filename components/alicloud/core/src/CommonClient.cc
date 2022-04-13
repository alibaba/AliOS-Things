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
#include <alibabacloud/core/CommonClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>
#include <alibabacloud/core/location/LocationClient.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#ifdef USE_AOS_TIME_POSIX_API
#include <posix/timer.h>
#endif
#include <alibabacloud/core/Utils.h>

namespace AlibabaCloud {

namespace {
const std::string SERVICE_NAME = "Common";
}

CommonClient::CommonClient(const Credentials &credentials,
                           const ClientConfiguration &configuration)
    : CoreClient(SERVICE_NAME, configuration),
      credentialsProvider_(
          std::make_shared<SimpleCredentialsProvider>(credentials)),
      signer_(std::make_shared<HmacSha1Signer>()) {}

CommonClient::CommonClient(
    const std::shared_ptr<CredentialsProvider> &credentialsProvider,
    const ClientConfiguration &configuration)
    : CoreClient(SERVICE_NAME, configuration),
      credentialsProvider_(credentialsProvider),
      signer_(std::make_shared<HmacSha1Signer>()) {}

CommonClient::CommonClient(const std::string &accessKeyId,
                           const std::string &accessKeySecret,
                           const ClientConfiguration &configuration)
    : CoreClient(SERVICE_NAME, configuration),
      credentialsProvider_(std::make_shared<SimpleCredentialsProvider>(
          accessKeyId, accessKeySecret)),
      signer_(std::make_shared<HmacSha1Signer>()) {}

CommonClient::~CommonClient() {}

CommonClient::JsonOutcome
CommonClient::makeRequest(const std::string &endpoint, const CommonRequest &msg,
                          HttpRequest::Method method) const {
  auto outcome = AttemptRequest(endpoint, msg, method);
  if (outcome.isSuccess())
    return JsonOutcome(
        std::string(outcome.result().body(), outcome.result().bodySize()));
  else
    return JsonOutcome(outcome.error());
}

CommonClient::CommonResponseOutcome
CommonClient::commonResponse(const CommonRequest &request) const {
  auto outcome = makeRequest(request.domain(), request, request.httpMethod());
  if (outcome.isSuccess())
    return CommonResponseOutcome(CommonResponse(outcome.result()));
  else
    return CommonResponseOutcome(Error(outcome.error()));
}

void CommonClient::commonResponseAsync(
    const CommonRequest &request, const CommonResponseAsyncHandler &handler,
    const std::shared_ptr<const AsyncCallerContext> &context) const {
  auto fn = [this, request, handler, context]() {
    handler(this, request, commonResponse(request), context);
  };

  asyncExecute(new Runnable(fn));
}

CommonClient::CommonResponseOutcomeCallable
CommonClient::commonResponseCallable(const CommonRequest &request) const {
  auto task = std::make_shared<std::packaged_task<CommonResponseOutcome()>>(
      [this, request]() { return this->commonResponse(request); });

  asyncExecute(new Runnable([task]() { (*task)(); }));
  return task->get_future();
}

HttpRequest CommonClient::buildHttpRequest(const std::string &endpoint,
                                           const ServiceRequest &msg,
                                           HttpRequest::Method method) const {
  return buildHttpRequest(endpoint, static_cast<const CommonRequest &>(msg), //lk change dynamic_cast to static_cast
                          method);
}

HttpRequest CommonClient::buildHttpRequest(const std::string &endpoint,
                                           const CommonRequest &msg,
                                           HttpRequest::Method method) const {
  if (msg.requestPattern() == CommonRequest::RpcPattern)
    return buildRpcHttpRequest(endpoint, msg, method);
  else
    return buildRoaHttpRequest(endpoint, msg, method);
}

HttpRequest
CommonClient::buildRoaHttpRequest(const std::string &endpoint,
                                  const CommonRequest &msg,
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

  auto params = msg.headerParameters();
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

  if (msg.headerParameter("Accept").empty()) {
    request.setHeader("Accept", "application/json");
  } else {
    request.setHeader("Accept", msg.headerParameter("Accept"));
  }

  std::stringstream ss;
  ss << msg.contentSize();
  request.setHeader("Content-Length", ss.str());
  if (msg.headerParameter("Content-Type").empty()) {
    request.setHeader("Content-Type", "application/octet-stream");
  } else {
    request.setHeader("Content-Type", msg.headerParameter("Content-Type"));
  }
  request.setHeader("Content-MD5",
                    ComputeContentMD5(msg.content(), msg.contentSize()));
  request.setBody(msg.content(), msg.contentSize());

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

HttpRequest
CommonClient::buildRpcHttpRequest(const std::string &endpoint,
                                  const CommonRequest &msg,
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

  auto params = msg.queryParameters();
  std::map<std::string, std::string> queryParams;
  for (const auto &p : params) {
    if (!p.second.empty())
      queryParams[p.first] = p.second;
  }

  queryParams["AccessKeyId"] = credentials.accessKeyId();
  queryParams["Format"] = "JSON";
  queryParams["RegionId"] = configuration().regionId();
  queryParams["SecurityToken"] = credentials.sessionToken();
  queryParams["SignatureMethod"] = signer_->name();
  queryParams["SignatureNonce"] = GenerateUuid();
  queryParams["SignatureVersion"] = signer_->version();
#ifdef USE_AOS_TIME_POSIX_API
    struct timespec currentTime;
    time_t t;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    t = currentTime.tv_nsec/1000000000 + currentTime.tv_sec;
#else
    std::time_t t = std::time(nullptr);
#endif
  std::stringstream ss;
#if defined(__GNUG__) && __GNUC__ < 5
  char tmbuff[26];
  strftime(tmbuff, 26, "%FT%TZ", std::gmtime(&t));
  ss << tmbuff;
#else
  ss << std::put_time(std::gmtime(&t), "%FT%TZ");
#endif
  queryParams["Timestamp"] = ss.str();
  queryParams["Version"] = msg.version();

  std::string bodyParamString;
  auto signParams = queryParams;
  auto bodyParams = msg.bodyParameters();
  for (const auto &p : bodyParams) {
    bodyParamString += "&";
    bodyParamString += (p.first + "=" + UrlEncode(p.second));
    signParams[p.first] = p.second;
  }

  std::stringstream plaintext;
  plaintext << HttpMethodToString(method) << "&" << UrlEncode(url.path()) << "&"
            << UrlEncode(canonicalizedQuery(signParams));
  queryParams["Signature"] =
      signer_->generate(plaintext.str(), credentials.accessKeySecret() + "&");

  std::stringstream queryString;
  for (const auto &p : queryParams)
    queryString << "&" << p.first << "=" << UrlEncode(p.second);
  url.setQuery(queryString.str().substr(1));

  HttpRequest request(url);
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

  request.setMethod(method);
  request.setHeader("Host", url.host());
  request.setHeader("x-sdk-client",
                    std::string("CPP/").append(ALIBABACLOUD_VERSION_STR));

  if (!bodyParamString.empty()) {
    request.setBody(bodyParamString.c_str() + 1, bodyParamString.size() - 1);
  }
  return request;
}

} // namespace AlibabaCloud
