
#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/Utils.h"
#include "alibabacloud/core/Config.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/CommonClient.h"
#include "alibabacloud/core/CommonResponse.h"
#include "alibabacloud/core/CommonRequest.h"
#include "alibabacloud/core/SimpleCredentialsProvider.h"


#include "alibabacloud/core/RoaServiceClient.h"
#include "alibabacloud/core/RpcServiceClient.h"

using namespace std;
using namespace AlibabaCloud;

const std::string unreachableDomain = "192.168.111.111";
const std::string timeoutErrorCode = "OperationTimeoutError";

const std::string defaultErrorMessage =
  "Timeout (connectTimeout: 5000ms, readTimeout: 10000ms) when connect or read data: GET http://192.168.111.111/";

long kClientConnectTimeout = 12;
long kClientReadTimeout = 234;
const std::string clientErrorMessage =
  "Timeout (connectTimeout: 12ms, readTimeout: 234ms) when connect or read data: GET http://192.168.111.111/";

long kRequestConnectTimeout = 33;
long kRequestReadTimeout = 724;
const std::string requestErrorMessage =
  "Timeout (connectTimeout: 33ms, readTimeout: 724ms) when connect or read data: GET http://192.168.111.111/";

TEST(timeout, DefaultCommonClientROA) {
  ClientConfiguration configuration("cn-hangzhou");
  CommonClient client("key", "secret", configuration);
  CommonRequest request(CommonRequest::RoaPattern);
  request.setScheme("http");
  request.setDomain(unreachableDomain);
  auto out = client.commonResponse(request);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage() == defaultErrorMessage);
}

TEST(timeout, DefaultCommonClientRPC) {
  ClientConfiguration configuration("cn-hangzhou");
  CommonClient client("key", "secret", configuration);
  CommonRequest request(CommonRequest::RpcPattern);
  request.setScheme("http");
  request.setDomain(unreachableDomain);
  auto out = client.commonResponse(request);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage().find(defaultErrorMessage) == 0);
}


TEST(timeout, clientCommonClientROA) {
  ClientConfiguration configuration("cn-hangzhou");
  configuration.setConnectTimeout(kClientConnectTimeout);
  configuration.setReadTimeout(kClientReadTimeout);
  CommonClient client("key", "secret", configuration);

  CommonRequest request(CommonRequest::RoaPattern);
  request.setScheme("http");
  request.setDomain(unreachableDomain);
  auto out = client.commonResponse(request);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage() == clientErrorMessage);
}

TEST(timeout, clientCommonClientRPC) {
  ClientConfiguration configuration("cn-hangzhou");
  configuration.setConnectTimeout(kClientConnectTimeout);
  configuration.setReadTimeout(kClientReadTimeout);
  CommonClient client("key", "secret", configuration);

  CommonRequest request(CommonRequest::RpcPattern);
  request.setScheme("http");
  request.setDomain(unreachableDomain);
  auto out = client.commonResponse(request);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage().find(clientErrorMessage) == 0);
}

TEST(timeout, requestCommonClientROA) {
  ClientConfiguration configuration("cn-hangzhou");
  CommonClient client("key", "secret", configuration);

  CommonRequest request(CommonRequest::RoaPattern);
  request.setScheme("http");
  request.setDomain(unreachableDomain);
  request.setConnectTimeout(kRequestConnectTimeout);
  request.setReadTimeout(kRequestReadTimeout);
  auto out = client.commonResponse(request);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage() == requestErrorMessage);
}

TEST(timeout, requestCommonClientRPC) {
  ClientConfiguration configuration("cn-hangzhou");
  CommonClient client("key", "secret", configuration);

  CommonRequest request(CommonRequest::RpcPattern);
  request.setScheme("http");
  request.setDomain(unreachableDomain);
  request.setConnectTimeout(kRequestConnectTimeout);
  request.setReadTimeout(kRequestReadTimeout);
  auto out = client.commonResponse(request);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage().find(requestErrorMessage) == 0);
}

TEST(timeout, clientRequestCommonClientROA) {
  ClientConfiguration configuration("cn-hangzhou");
  configuration.setConnectTimeout(kClientConnectTimeout);
  configuration.setReadTimeout(kClientReadTimeout);
  CommonClient client("key", "secret", configuration);

  CommonRequest request(CommonRequest::RoaPattern);
  request.setScheme("http");
  request.setDomain(unreachableDomain);
  request.setConnectTimeout(kRequestConnectTimeout);
  request.setReadTimeout(kRequestReadTimeout);
  auto out = client.commonResponse(request);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage() == requestErrorMessage);
}

TEST(timeout, clientRequestCommonClientRPC) {
  ClientConfiguration configuration("cn-hangzhou");
  configuration.setConnectTimeout(kClientConnectTimeout);
  configuration.setReadTimeout(kClientReadTimeout);
  CommonClient client("key", "secret", configuration);

  CommonRequest request(CommonRequest::RpcPattern);
  request.setConnectTimeout(kRequestConnectTimeout);
  request.setReadTimeout(kRequestReadTimeout);

  request.setScheme("http");
  request.setDomain(unreachableDomain);
  auto out = client.commonResponse(request);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage().find(requestErrorMessage) == 0);
}

class TestRoaClient : public RoaServiceClient {
 public:
  TestRoaClient(const std::string & servicename, const std::shared_ptr<CredentialsProvider> &credentialsProvider,
    const ClientConfiguration &configuration,
    const std::shared_ptr<Signer> &signer = std::make_shared<HmacSha1Signer>()):
  RoaServiceClient(servicename, credentialsProvider, configuration, signer)
  {}

  JsonOutcome makeRequest(const std::string &endpoint, const RoaServiceRequest &msg, HttpRequest::Method method = HttpRequest::Method::Get)const {
    return RoaServiceClient::makeRequest(endpoint, msg, method);
  }
};

TEST(timeout, roaClientDefault) {
  ClientConfiguration configuration("cn-hangzhou");
  const Credentials credentials("key", "secret");
  TestRoaClient client("any-service", std::make_shared<SimpleCredentialsProvider>(credentials), configuration);
  RoaServiceRequest request("product", "version");
  request.setScheme("http");
  auto out = client.makeRequest(unreachableDomain, request, HttpRequest::Method::Get);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage().find(defaultErrorMessage) == 0);
}

TEST(timeout, roaClientclient) {
  ClientConfiguration configuration("cn-hangzhou");
  configuration.setConnectTimeout(kClientConnectTimeout);
  configuration.setReadTimeout(kClientReadTimeout);
  const Credentials credentials("key", "secret");
  TestRoaClient client("any-service", std::make_shared<SimpleCredentialsProvider>(credentials), configuration);
  RoaServiceRequest request("product", "version");
  request.setScheme("http");
  auto out = client.makeRequest(unreachableDomain, request, HttpRequest::Method::Get);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage().find(clientErrorMessage) == 0);
}

TEST(timeout, roaClientrequest) {
  ClientConfiguration configuration("cn-hangzhou");
  const Credentials credentials("key", "secret");
  TestRoaClient client("any-service", std::make_shared<SimpleCredentialsProvider>(credentials), configuration);
  RoaServiceRequest request("product", "version");
  request.setScheme("http");
  request.setConnectTimeout(kRequestConnectTimeout);
  request.setReadTimeout(kRequestReadTimeout);
  auto out = client.makeRequest(unreachableDomain, request, HttpRequest::Method::Get);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage().find(requestErrorMessage) == 0);
}

TEST(timeout, roaClientclientrequest) {
  ClientConfiguration configuration("cn-hangzhou");
  configuration.setConnectTimeout(kClientConnectTimeout);
  configuration.setReadTimeout(kClientReadTimeout);
  const Credentials credentials("key", "secret");
  TestRoaClient client("any-service", std::make_shared<SimpleCredentialsProvider>(credentials), configuration);
  RoaServiceRequest request("product", "version");
  request.setScheme("http");
  request.setConnectTimeout(kRequestConnectTimeout);
  request.setReadTimeout(kRequestReadTimeout);
  auto out = client.makeRequest(unreachableDomain, request, HttpRequest::Method::Get);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage().find(requestErrorMessage) == 0);
}


class TestRpcClient : public RpcServiceClient {
 public:
  TestRpcClient(const std::string & servicename, const std::shared_ptr<CredentialsProvider> &credentialsProvider,
  const ClientConfiguration &configuration,
  const std::shared_ptr<Signer> &signer = std::make_shared<HmacSha1Signer>()):
  RpcServiceClient(servicename, credentialsProvider, configuration, signer)
  {}

  JsonOutcome makeRequest(const std::string &endpoint, const RpcServiceRequest &msg, HttpRequest::Method method = HttpRequest::Method::Get)const {
    return RpcServiceClient::makeRequest(endpoint, msg, method);
  }
};

TEST(timeout, rpcClientDefault) {
  ClientConfiguration configuration("cn-hangzhou");
  const Credentials credentials("key", "secret");
  TestRpcClient client("any-service", std::make_shared<SimpleCredentialsProvider>(credentials), configuration);
  RpcServiceRequest request("product", "version", "aciton");
  request.setScheme("http");
  auto out = client.makeRequest(unreachableDomain, request, HttpRequest::Method::Get);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage().find(defaultErrorMessage) == 0);
}

TEST(timeout, rpcClientclient) {
  ClientConfiguration configuration("cn-hangzhou");
  configuration.setConnectTimeout(kClientConnectTimeout);
  configuration.setReadTimeout(kClientReadTimeout);
  const Credentials credentials("key", "secret");
  TestRpcClient client("any-service", std::make_shared<SimpleCredentialsProvider>(credentials), configuration);
  RpcServiceRequest request("product", "version", "aciton");
  request.setScheme("http");
  auto out = client.makeRequest(unreachableDomain, request, HttpRequest::Method::Get);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage().find(clientErrorMessage) == 0);
}

TEST(timeout, rpcClientrequest) {
  ClientConfiguration configuration("cn-hangzhou");
  const Credentials credentials("key", "secret");
  TestRpcClient client("any-service", std::make_shared<SimpleCredentialsProvider>(credentials), configuration);
  RpcServiceRequest request("product", "version", "aciton");
  request.setScheme("http");
  request.setConnectTimeout(kRequestConnectTimeout);
  request.setReadTimeout(kRequestReadTimeout);
  auto out = client.makeRequest(unreachableDomain, request, HttpRequest::Method::Get);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage().find(requestErrorMessage) == 0);
}

TEST(timeout, rpcClientclientrequest) {
  ClientConfiguration configuration("cn-hangzhou");
  configuration.setConnectTimeout(kClientConnectTimeout);
  configuration.setReadTimeout(kClientReadTimeout);
  const Credentials credentials("key", "secret");
  TestRpcClient client("any-service", std::make_shared<SimpleCredentialsProvider>(credentials), configuration);
  RpcServiceRequest request("product", "version", "aciton");
  request.setScheme("http");
  request.setConnectTimeout(kRequestConnectTimeout);
  request.setReadTimeout(kRequestReadTimeout);
  auto out = client.makeRequest(unreachableDomain, request, HttpRequest::Method::Get);
  EXPECT_TRUE(out.error().errorCode() == timeoutErrorCode);
  EXPECT_TRUE(out.error().errorMessage().find(requestErrorMessage) == 0);
}
