#include <iostream>
#include <stdio.h>

#include "utils.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "alibabacloud/core/Url.h"
#include "alibabacloud/core/HttpRequest.h"
#include "alibabacloud/core/HttpClient.h"
#include "../src/CurlHttpClient.h"

using namespace std;
using ::testing::_;
using ::testing::NiceMock;
using ::testing::DefaultValue;

using namespace AlibabaCloud;

using namespace std;

class mockCurlHttpClient : public CurlHttpClient {
 public:
  MOCK_METHOD1(makeRequest, HttpResponseOutcome (const HttpRequest &request));
};

TEST(CurlHttpClient, connectTimeout) {
  CurlHttpClient client;
  HttpRequest request;

  Url url;
  url.setScheme("http");
  url.setHost("192.168.100.100");

  request.setMethod(HttpRequest::Method::Get);
  request.setUrl(url);
  request.setConnectTimeout(100);
  request.setReadTimeout(1000);
  request.setHeader("Content-Type", "text/html");
  HttpClient::HttpResponseOutcome out = client.makeRequest(request);
  string errmsg = "Timeout (connectTimeout: 100ms, readTimeout: 1000ms) when connect or read data: GET http://192.168.100.100/";
  EXPECT_TRUE(out.error().errorCode() == "OperationTimeoutError");
  EXPECT_TRUE(out.error().errorMessage() == errmsg);
}

TEST(CurlHttpClient, defaultTimeout) {
  CurlHttpClient client;
  HttpRequest request;

  Url url;
  url.setScheme("http");
  url.setHost("192.168.100.100");

  request.setMethod(HttpRequest::Method::Get);
  request.setUrl(url);
  request.setHeader("Content-Type", "text/html");
  HttpClient::HttpResponseOutcome out = client.makeRequest(request);
  string errmsg = "Timeout (connectTimeout: 5000ms, readTimeout: 10000ms) when connect or read data: GET http://192.168.100.100/";
  EXPECT_TRUE(out.error().errorCode() == "OperationTimeoutError");
  EXPECT_TRUE(out.error().errorMessage() == errmsg);
}

TEST(CurlHttpClient, readTimeout) {
  CurlHttpClient client;
  HttpRequest request;

  Url url;
  url.setScheme("http");
  url.setHost("127.0.0.1");
  url.setPort(8021);
  url.setPath("/readTimeoutTest");
  url.setQuery("timeout=500");

  request.setMethod(HttpRequest::Method::Get);
  request.setUrl(url);
  request.setConnectTimeout(233);
  request.setReadTimeout(500);
  request.setHeader("Content-Type", "text/html");
  HttpClient::HttpResponseOutcome out = client.makeRequest(request);
  string errmsg = "Timeout (connectTimeout: 233ms, readTimeout: 500ms) when connect or read data: GET http://127.0.0.1:8021/readTimeoutTest?timeout=500";

  EXPECT_TRUE(out.error().errorCode() == "OperationTimeoutError");
  EXPECT_TRUE(out.error().errorMessage() == errmsg);
}

TEST(CurlHttpClient, http_get) {
  CurlHttpClient client;
  HttpRequest request;

  Url url;
  url.setHost("127.0.0.1");
  url.setPort(8021);
  url.setPath("/anypath");
  url.setQuery("k1=v1&k2=v2");

  request.setMethod(HttpRequest::Method::Get);
  request.setUrl(url);
  request.setHeader("Content-Type", "text/html");
  HttpClient::HttpResponseOutcome out = client.makeRequest(request);
  EXPECT_TRUE(std::string(out.result().body()) == "{\"k1\":\"v1\",\"k2\":\"v2\"}");
}

TEST(CurlHttpClient, http_post) {
  CurlHttpClient client;
  HttpRequest request;
  std::string test_body = "any-body";
  Url url;
  url.setHost("127.0.0.1");
  url.setPort(8021);
  url.setPath("/anypath");
  url.setQuery("k1=v1&k2=v2");

  request.setMethod(HttpRequest::Method::Post);
  request.setUrl(url);
  request.setHeader("Content-Type", "text/html");
  request.setBody(test_body.c_str(), test_body.size());
  HttpClient::HttpResponseOutcome out = client.makeRequest(request);
  EXPECT_TRUE(std::string(out.result().body()) == "POST: " + test_body);
}

TEST(CurlHttpClient, http_put) {
  CurlHttpClient client;
  HttpRequest request;
  std::string test_body = "any-body";
  Url url;
  url.setHost("127.0.0.1");
  url.setPort(8021);
  url.setPath("/anypath");
  url.setQuery("k1=v1&k2=v2");

  request.setMethod(HttpRequest::Method::Put);
  request.setUrl(url);
  request.setHeader("Content-Type", "text/html");
  request.setBody(test_body.c_str(), test_body.size());
  HttpClient::HttpResponseOutcome out = client.makeRequest(request);
  EXPECT_TRUE(std::string(out.result().body()) == "PUT: " + test_body);
}

TEST(CurlHttpClient, http_delete) {
  CurlHttpClient client;
  HttpRequest request;
  std::string test_body = "any-body";
  Url url;
  url.setHost("127.0.0.1");
  url.setPort(8021);
  url.setPath("/anypath");
  url.setQuery("k1=v1&k2=v2");

  request.setMethod(HttpRequest::Method::Delete);
  request.setUrl(url);
  request.setHeader("Content-Type", "text/html");
  request.setBody(test_body.c_str(), test_body.size());
  HttpClient::HttpResponseOutcome out = client.makeRequest(request);
  EXPECT_TRUE(std::string(out.result().body()) == "DELETE: " + test_body);
}

TEST(CurlHttpClient, netWorkError) {
  CurlHttpClient client;
  HttpRequest request;
  HttpClient::HttpResponseOutcome out = client.makeRequest(request);
  EXPECT_TRUE(out.error().errorCode() == "NetworkError");
  EXPECT_TRUE(out.error().errorMessage().find("Failed to connect to host or proxy: GET ") == 0);
}

TEST(CurlHttpClient, netWorkErrorWithHttpProxy) {
  const std::string hostname = "hostname";
  const std::string user = "user";
  const std::string password = "password";
  uint16_t port = 12345;
  const NetworkProxy proxy(NetworkProxy::Http, hostname, port, user, password);

  CurlHttpClient client;
  client.setProxy(proxy);

  HttpRequest request;
  HttpClient::HttpResponseOutcome out = client.makeRequest(request);
  EXPECT_TRUE(out.error().errorCode() == "NetworkError");
  EXPECT_TRUE(out.error().errorMessage().find("Failed to connect to host or proxy: GET ") == 0);
}

TEST(CurlHttpClient, netWorkErrorWithSocks5Proxy) {
  const std::string hostname = "hostname";
  const std::string user = "user";
  const std::string password = "password";
  uint16_t port = 12345;
  const NetworkProxy proxy(NetworkProxy::Socks5, hostname, port, user, password);

  CurlHttpClient client;
  client.setProxy(proxy);

  HttpRequest request;
  HttpClient::HttpResponseOutcome out = client.makeRequest(request);
  EXPECT_TRUE(out.error().errorCode() == "NetworkError");
  EXPECT_TRUE(out.error().errorMessage().find("Failed to connect to host or proxy: GET ") == 0);
}

TEST(CurlHttpClient, mock) {
  CurlHttpClient client;
  HttpRequest request;

  string body = "1234567";
  HttpResponse response(request);
  response.setStatusCode(200);
  response.setBody(body.c_str(), body.size());

  NiceMock<mockCurlHttpClient> mclient;

  HttpClient::HttpResponseOutcome res = HttpClient::HttpResponseOutcome(response);
  HttpClient::HttpResponseOutcome out1;

  out1.error().setErrorCode("111");
  out1.error().setErrorMessage("this is a mock test");

  DefaultValue<HttpClient::HttpResponseOutcome>::Set(res);

  HttpRequest req;

  EXPECT_CALL(mclient, makeRequest(_));

  HttpClient::HttpResponseOutcome o = mclient.makeRequest(request);
  EXPECT_TRUE(res.result().body() == body);
}
