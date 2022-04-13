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

#include "CurlHttpClient.h"
#include <alibabacloud/core/Utils.h>
#include <cassert>
#include <sstream>
#include <string.h>
#include <string>
#include <vector>

namespace AlibabaCloud {

namespace {

typedef struct {
  const char *data;
  const char *pos;
  const char *last;
} uploadContext;

static size_t readCallback(void *ptr, size_t size, size_t nmemb, void *stream) {
  uploadContext *ctx = (uploadContext *)stream;
  size_t len = 0;

  if (ctx->pos >= ctx->last) {
    return 0;
  }
  if ((size == 0) || (nmemb == 0) || (size * nmemb < 1)) {
    return 0;
  }
  len = ctx->last - ctx->pos;
  if (len > size * nmemb) {
    len = size * nmemb;
  }
  memcpy(ptr, ctx->pos, len);
  ctx->pos += len;
  return len;
}

size_t recvBody(char *ptr, size_t size, size_t nmemb, void *userdata) {
  std::ostringstream &out = *static_cast<std::ostringstream *>(userdata);
  out << std::string(ptr, nmemb * size);
  return nmemb * size;
}

size_t recvHeaders(char *buffer, size_t size, size_t nitems, void *userdata) {
  HttpResponse *response = static_cast<HttpResponse *>(userdata);
  std::string line(buffer);
  auto pos = line.find(':');
  if (pos != line.npos) {
    std::string name = line.substr(0, pos);
    std::string value = line.substr(pos + 2);
    size_t p = 0;
    if ((p = value.rfind('\r')) != value.npos)
      value[p] = '\0';
    response->setHeader(name, value);
  }
  return nitems * size;
}

void setCUrlProxy(CURL *curlHandle, const NetworkProxy &proxy) {
  if (proxy.type() == NetworkProxy::Type::None)
    return;

  long type;
  switch (proxy.type()) {
  case NetworkProxy::Type::Socks5:
    type = CURLPROXY_SOCKS5;
    break;
  case NetworkProxy::Type::Http:
  default:
    type = CURLPROXY_HTTP;
    break;
  }
  curl_easy_setopt(curlHandle, CURLOPT_PROXYTYPE, type);

  std::ostringstream out;
  out << proxy.hostName() << ":" << proxy.port();
  curl_easy_setopt(curlHandle, CURLOPT_PROXY, out.str().c_str());

  if (!proxy.user().empty()) {
    out.clear();
    out << proxy.user() << ":" << proxy.password();
    curl_easy_setopt(curlHandle, CURLOPT_PROXYUSERPWD, out.str().c_str());
  }
}
} // namespace

CurlHttpClient::CurlHttpClient()
    : HttpClient(), curlHandle_(curl_easy_init()) {}

CurlHttpClient::~CurlHttpClient() { curl_easy_cleanup(curlHandle_); }

HttpClient::HttpResponseOutcome
CurlHttpClient::makeRequest(const HttpRequest &request) {
  curl_easy_reset(curlHandle_);
  HttpResponse response(request);

  long connect_timeout = request.connectTimeout();
  long read_timeout = request.readTimeout();

  std::string url = request.url().toString();
  switch (request.method()) {
  case HttpRequest::Method::Get:
    break;
  case HttpRequest::Method::Post: {
    if (request.bodySize() > 0) {
      curl_easy_setopt(curlHandle_, CURLOPT_POSTFIELDS, request.body());
    } else {
      curl_easy_setopt(curlHandle_, CURLOPT_POSTFIELDS, "");
    }
  } break;

  case HttpRequest::Method::Put: {
    uploadContext *ctx = (uploadContext *)malloc(sizeof(uploadContext));
    // this is impossible, as the size is 24 Bytes
    if (ctx == nullptr) {
      return HttpResponseOutcome(
          Error("MemoryAllocateError",
                "There is not enough memory for http transfer."));
    }
    ctx->data = request.body();
    ctx->pos = request.body();
    ctx->last = ctx->pos + request.bodySize();

    curl_easy_setopt(curlHandle_, CURLOPT_READFUNCTION, readCallback);
    curl_easy_setopt(curlHandle_, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curlHandle_, CURLOPT_READDATA, ctx);
  } break;

  case HttpRequest::Method::Delete: {
    curl_easy_setopt(curlHandle_, CURLOPT_CUSTOMREQUEST, "DELETE");
    if (request.bodySize() > 0) {
      curl_easy_setopt(curlHandle_, CURLOPT_POSTFIELDS, request.body());
    } else {
      curl_easy_setopt(curlHandle_, CURLOPT_POSTFIELDS, "");
    }
  } break;

  default:
    break;
  }
  //printf("url string: %s\n", url.c_str());
  //printf("request.body: %s\n", request.body());
  curl_easy_setopt(curlHandle_, CURLOPT_URL, url.c_str());
  if (rejectUnauthorized()) {
    curl_easy_setopt(curlHandle_, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curlHandle_, CURLOPT_SSL_VERIFYHOST, 2L);
  } else {
    curl_easy_setopt(curlHandle_, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curlHandle_, CURLOPT_SSL_VERIFYHOST, 0L);
  }
  curl_easy_setopt(curlHandle_, CURLOPT_HEADERDATA, &response);
  curl_easy_setopt(curlHandle_, CURLOPT_HEADERFUNCTION, recvHeaders);

  if (connect_timeout > 0) {
    curl_easy_setopt(curlHandle_, CURLOPT_CONNECTTIMEOUT_MS, connect_timeout);
  }

  if (read_timeout > 0) {
    curl_easy_setopt(curlHandle_, CURLOPT_TIMEOUT_MS, read_timeout);
  }

  curl_slist *list = nullptr;
  auto headers = request.headers();
  for (const auto &p : headers) {
    std::string str = p.first;
    str.append(": ").append(p.second);
    list = curl_slist_append(list, str.c_str());
  }
  curl_easy_setopt(curlHandle_, CURLOPT_HTTPHEADER, list);
  std::ostringstream out;
  curl_easy_setopt(curlHandle_, CURLOPT_WRITEDATA, &out);
  curl_easy_setopt(curlHandle_, CURLOPT_WRITEFUNCTION, recvBody);
  setCUrlProxy(curlHandle_, proxy());

  if (GetEnv("DEBUG") == "sdk") {
    curl_easy_setopt(curlHandle_, CURLOPT_VERBOSE, 1L);
  }

  CURLcode res = curl_easy_perform(curlHandle_);
  switch (res) {
  case CURLE_OK: {
    long response_code;
    curl_easy_getinfo(curlHandle_, CURLINFO_RESPONSE_CODE, &response_code);
    response.setStatusCode(response_code);
    response.setBody(out.str().c_str(), out.str().length());
    return HttpResponseOutcome(response);
  }
  case CURLE_SSL_CONNECT_ERROR:
    return HttpResponseOutcome(
        Error("SSLConnectError",
              "A problem occurred somewhere in the SSL/TLS handshake."));
  case CURLE_OPERATION_TIMEDOUT:
    return HttpResponseOutcome(
        Error("OperationTimeoutError",
              "Timeout (connectTimeout: " + std::to_string(connect_timeout) +
                  "ms, readTimeout: " + std::to_string(read_timeout) +
                  "ms) when connect or read data: " +
                  HttpMethodToString(request.method()) + " " +
                  request.url().toString()));

  default: {
    return HttpResponseOutcome(
        Error("NetworkError", "Failed to connect to host or proxy: " +
                                  HttpMethodToString(request.method()) + " " +
                                  request.url().toString()));
  }
  }
  curl_slist_free_all(list);
}

} // namespace AlibabaCloud
