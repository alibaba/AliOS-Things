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
#include <alibabacloud/core/RpcServiceClient.h>
#include <alibabacloud/core/Utils.h>
#include <iomanip>
#include <sstream>
#ifdef USE_AOS_TIME_POSIX_API
#include <posix/timer.h>
#endif

namespace AlibabaCloud {

RpcServiceClient::RpcServiceClient(
    const std::string &servicename,
    const std::shared_ptr<CredentialsProvider> &credentialsProvider,
    const ClientConfiguration &configuration,
    const std::shared_ptr<Signer> &signer)
    : CoreClient(servicename, configuration),
      credentialsProvider_(credentialsProvider), signer_(signer) {}

RpcServiceClient::~RpcServiceClient() {}

RpcServiceClient::JsonOutcome
RpcServiceClient::makeRequest(const std::string &endpoint,
                              const RpcServiceRequest &request,
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
RpcServiceClient::buildHttpRequest(const std::string &endpoint,
                                   const ServiceRequest &msg,
                                   HttpRequest::Method method) const {
  return buildHttpRequest(endpoint,
                          static_cast<const RpcServiceRequest &>(msg), method); //lk change dynamic_cast to static_cast
}

HttpRequest
RpcServiceClient::buildHttpRequest(const std::string &endpoint,
                                   const RpcServiceRequest &msg,
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

  std::map<std::string, std::string> signParams;

  auto params = msg.parameters();
  for (const auto &p : params) {
    if (!p.second.empty())
      signParams[p.first] = p.second;
  }

  signParams["AccessKeyId"] = credentials.accessKeyId();
  signParams["Format"] = "JSON";
  signParams["RegionId"] = configuration().regionId();
  signParams["SecurityToken"] = credentials.sessionToken();
  signParams["SignatureMethod"] = signer_->name();
  signParams["SignatureNonce"] = GenerateUuid();
  signParams["SignatureVersion"] = signer_->version();
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
  signParams["Timestamp"] = ss.str();
  signParams["Version"] = msg.version();

  std::map<std::string, std::string> query;
  for (const auto &p : signParams) {
    query[p.first] = p.second;
  }

  auto body_params = msg.bodyParameters();
  for (const auto &p : body_params) {
    signParams[p.first] = p.second;
  }

  std::stringstream plaintext;
  plaintext << HttpMethodToString(method) << "&" << UrlEncode(url.path()) << "&"
            << UrlEncode(canonicalizedQuery(signParams));
  query["Signature"] =
      signer_->generate(plaintext.str(), credentials.accessKeySecret() + "&");

  std::stringstream queryString;
  for (const auto &p : query)
    queryString << "&" << p.first << "=" << UrlEncode(p.second);
  url.setQuery(queryString.str().substr(1));

  HttpRequest request(url);
  if (msg.connectTimeout() != kInvalidTimeout) {
    request.setConnectTimeout(msg.connectTimeout());
  } else {
    request.setConnectTimeout(configuration().connectTimeout());
  }

  for (const auto &h : msg.headers()) {
    if (!h.second.empty()) {
      request.setHeader(h.first, h.second);
    }
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
  std::stringstream tmp;
  for (const auto &p : body_params)
    tmp << "&" << p.first << "=" << UrlEncode(p.second);
  if (tmp.str().length() > 0) {
    std::string body = tmp.str().substr(1);
    request.setBody(body.c_str(), body.length());
  }
  return request;
}

} // namespace AlibabaCloud
