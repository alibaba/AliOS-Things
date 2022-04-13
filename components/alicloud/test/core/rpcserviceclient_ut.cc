#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "alibabacloud/core/SimpleCredentialsProvider.h"
#include "alibabacloud/core/RpcServiceClient.h"
#include "alibabacloud/core/HttpMessage.h"
#include "alibabacloud/core/HttpClient.h"
#include "alibabacloud/core/HttpRequest.h"

using namespace std;
using namespace AlibabaCloud;

using ::testing::_;
using ::testing::DefaultValue;

namespace AlibabaCloud {
  class mockRpcServiceClient : public RpcServiceClient {
   public:
    mockRpcServiceClient(const std::string & servicename, const std::shared_ptr<CredentialsProvider> &credentialsProvider,
      const ClientConfiguration &configuration,
      const std::shared_ptr<Signer> &signer = std::make_shared<HmacSha1Signer>()):
    RpcServiceClient(servicename, credentialsProvider, configuration, signer)
    {}
    MOCK_CONST_METHOD3(AttemptRequest, HttpClient::HttpResponseOutcome (const std::string & endpoint, const ServiceRequest &request, HttpRequest::Method method));
    using RpcServiceClient::makeRequest;
    using RpcServiceClient::buildHttpRequest;

  };

  class mockServiceRequest: public ServiceRequest {
   public:
    mockServiceRequest(const std::string &product, const std::string &version):
    ServiceRequest(product, version){}

    using ServiceRequest::addParameter;
    using ServiceRequest::parameter;
    using ServiceRequest::removeParameter;
    using ServiceRequest::setContent;
    using ServiceRequest::setParameter;
    using ServiceRequest::setParameters;
    using ServiceRequest::setResourcePath;
    using ServiceRequest::setProduct;
    using ServiceRequest::setVersion;
  };
}

TEST(RpcServiceClient, basic) {
  const ClientConfiguration config;

  std::string key    = "fake-key";
  std::string secret = "fake-secret";
  std::string token  = "fake-token";

  const Credentials credentials(key, secret, token);

  mockRpcServiceClient client("ecs", std::make_shared<SimpleCredentialsProvider>(credentials), config);

  const string product = "ECS";
  const string version = "1.0";
  const string action = "fake-action";
  const string endpoint = "test-cn-shanghai";

  RpcServiceRequest req(product, version, action);

  req.setScheme("");
  HttpRequest http_req = client.buildHttpRequest(endpoint, req, HttpRequest::Method::Get);
  EXPECT_TRUE(http_req.header("x-sdk-client") == std::string("CPP/").append(ALIBABACLOUD_VERSION_STR));
  EXPECT_TRUE(http_req.header("Host") == endpoint);
  EXPECT_TRUE(http_req.method() == HttpRequest::Method::Get);
  EXPECT_TRUE(http_req.url().query().find("AccessKeySecret") == string::npos);
  EXPECT_TRUE(http_req.url().query().find("AccessKeyId=fake-key") != string::npos);
  EXPECT_TRUE(http_req.url().query().find("&SecurityToken=fake-token") != string::npos);
  EXPECT_TRUE(http_req.url().query().find("&Action=fake-action") != string::npos);
  EXPECT_TRUE(http_req.url().query().find("&Format=JSON") != string::npos);
  EXPECT_TRUE(http_req.url().query().find("&RegionId=cn-hangzhou") != string::npos);
  EXPECT_TRUE(http_req.url().query().find("&Signature=") != string::npos);
  EXPECT_TRUE(http_req.url().query().find("&SignatureMethod=HMAC-SHA1") != string::npos);
  EXPECT_TRUE(http_req.url().query().find("&SignatureNonce=") != string::npos);
  EXPECT_TRUE(http_req.url().query().find("&SignatureVersion=1.0") != string::npos);
  EXPECT_TRUE(http_req.url().query().find("&Version=1.0") != string::npos);
  EXPECT_TRUE(http_req.url().query().find("&Timestamp=") != string::npos);
  EXPECT_TRUE(http_req.url().scheme() == "https");

  req.setScheme("http");
  http_req = client.buildHttpRequest(endpoint, req, HttpRequest::Method::Get);
  EXPECT_TRUE(http_req.url().scheme() == "http");
}

TEST(RpcServiceClient, mock) {
  const string product = "ECS";
  const string version = "1.0";
  const string action = "fake-action";
  const string endpoint = "test-cn-shanghai";
  const string servicename = "fake-service-name";

  const ClientConfiguration configuration;

  std::string key    = "fake-key";
  std::string secret = "fake-secret";
  std::string token  = "fake-token";

  const Credentials credentials(key, secret, token);

  RpcServiceRequest rpc_req(product, version, action);
  HttpClient::HttpResponseOutcome xout(Error("any-error-code", "any-error-message"));
  DefaultValue<HttpClient::HttpResponseOutcome>::Set(xout);


  mockRpcServiceClient client(servicename, std::make_shared<SimpleCredentialsProvider>(credentials), configuration);
  EXPECT_CALL(client, AttemptRequest(_, _, _));

  RpcServiceClient::JsonOutcome out = client.makeRequest(endpoint, rpc_req, HttpRequest::Method::Get);
  EXPECT_TRUE(out.error().errorCode() == "any-error-code");
}

TEST(RpcServiceClient, mock_1) {
  const string product = "ECS";
  const string version = "1.0";
  const string action = "fake-action";
  const string endpoint = "test-cn-shanghai";
  const string servicename = "fake-service-name";

  const ClientConfiguration configuration;

  std::string key    = "fake-key";
  std::string secret = "fake-secret";
  std::string token  = "fake-token";

  const Credentials credentials(key, secret, token);

  RpcServiceRequest rpc_req(product, version, action);

  Url url;
  url.setHost("example.com");
  HttpRequest req(url, HttpRequest::Method::Post);
  HttpResponse res(req);
  res.setBody("any-body", 8);

  HttpClient::HttpResponseOutcome xout(res);
  DefaultValue<HttpClient::HttpResponseOutcome>::Set(xout);

  mockRpcServiceClient client(servicename, std::make_shared<SimpleCredentialsProvider>(credentials), configuration);
  EXPECT_CALL(client, AttemptRequest(_, _, _));

  RpcServiceClient::JsonOutcome out = client.makeRequest(endpoint, rpc_req, HttpRequest::Method::Get);
  EXPECT_TRUE(out.error().errorCode().empty());
  EXPECT_TRUE(out.result() == "any-body");
}


