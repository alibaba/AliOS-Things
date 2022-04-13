#include <iostream>
#include <stdio.h>
#include "utils.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/SimpleCredentialsProvider.h"
#include "alibabacloud/core/sts/StsClient.h"

using ::testing::_;
using ::testing::DefaultValue;

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Sts;

namespace {

  class mockStsClient : public StsClient {
   public:
    mockStsClient(const string& key, const string& secret, const ClientConfiguration& cfg):
    StsClient(key, secret, cfg) {}
    mockStsClient(const Credentials& credentials, const ClientConfiguration& cfg):
    StsClient(credentials, cfg) {}
    mockStsClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration):
    StsClient(credentialsProvider, configuration) {}
    MOCK_CONST_METHOD3(makeRequest, RpcServiceClient::JsonOutcome (const std::string &endpoint, const RpcServiceRequest &msg, HttpRequest::Method method));
  };


  TEST(StsClient, getCallerIdentity_ak) {
    ClientConfiguration configuration;
    RpcServiceClient::JsonOutcome xout("{\"Request\":\"test-req-id\",\"AccountId\":\"123456789\",\"Arn\":\"any-arn\",\"UserId\":\"any-id\"}");
    mockStsClient client("mock-key", "mock-secret", configuration);
    DefaultValue<RpcServiceClient::JsonOutcome>::Set(xout);
    EXPECT_CALL(client, makeRequest(_, _, _));
    Model::GetCallerIdentityRequest req;
    StsClient::GetCallerIdentityOutcome out = client.getCallerIdentity(req);
    EXPECT_TRUE(out.error().errorCode().empty());
    EXPECT_TRUE(out.result().accountId() == "123456789");
    EXPECT_TRUE(out.result().userId() == "any-id");
    EXPECT_TRUE(out.result().arn() == "any-arn");
  }

  TEST(StsClient, getCallerIdentity_credentials) {
    ClientConfiguration configuration;
    RpcServiceClient::JsonOutcome xout("{\"Request\":\"test-req-id\",\"AccountId\":\"123456789\",\"Arn\":\"any-arn\",\"UserId\":\"any-id\"}");
    Credentials credentials("key", "secret");
    mockStsClient client(credentials, configuration);
    DefaultValue<RpcServiceClient::JsonOutcome>::Set(xout);
    EXPECT_CALL(client, makeRequest(_, _, _));
    Model::GetCallerIdentityRequest req;
    StsClient::GetCallerIdentityOutcome out = client.getCallerIdentity(req);
    EXPECT_TRUE(out.error().errorCode().empty());
    EXPECT_TRUE(out.result().accountId() == "123456789");
    EXPECT_TRUE(out.result().userId() == "any-id");
    EXPECT_TRUE(out.result().arn() == "any-arn");
  }

  TEST(StsClient, getCallerIdentity_credentials_ptr) {

    ClientConfiguration configuration;
    RpcServiceClient::JsonOutcome xout("{\"Request\":\"test-req-id\",\"AccountId\":\"123456789\",\"Arn\":\"any-arn\",\"UserId\":\"any-id\"}");
    Credentials credentials("key", "secret");
    mockStsClient client(std::make_shared<SimpleCredentialsProvider>(credentials), configuration);
    DefaultValue<RpcServiceClient::JsonOutcome>::Set(xout);
    EXPECT_CALL(client, makeRequest(_, _, _));
    Model::GetCallerIdentityRequest req;
    StsClient::GetCallerIdentityOutcome out = client.getCallerIdentity(req);
    EXPECT_TRUE(out.error().errorCode().empty());
    EXPECT_TRUE(out.result().accountId() == "123456789");
    EXPECT_TRUE(out.result().userId() == "any-id");
    EXPECT_TRUE(out.result().arn() == "any-arn");
  }

  TEST(StsClient, getCallerIdentity_ak_error) {
    InitializeSdk();
    ClientConfiguration configuration;
    RpcServiceClient::JsonOutcome xout(Error("any-error-code", "any-error-message"));
    mockStsClient client("mock-key", "mock-secret", configuration);
    DefaultValue<RpcServiceClient::JsonOutcome>::Set(xout);
    EXPECT_CALL(client, makeRequest(_, _, _));
    Model::GetCallerIdentityRequest req;
    StsClient::GetCallerIdentityOutcome out = client.getCallerIdentity(req);
    EXPECT_TRUE(out.error().errorCode() == "any-error-code");
    ShutdownSdk();
  }

  TEST(StsClient, getCallerIdentity_callable) {
    InitializeSdk();
    ClientConfiguration configuration;
    RpcServiceClient::JsonOutcome xout("{\"Request\":\"test-req-id\",\"AccountId\":\"123456789\",\"Arn\":\"any-arn\",\"UserId\":\"any-id\"}");
    mockStsClient client("mock-key", "mock-secret", configuration);
    DefaultValue<RpcServiceClient::JsonOutcome>::Set(xout);
    EXPECT_CALL(client, makeRequest(_, _, _));
    Model::GetCallerIdentityRequest req;
    StsClient::GetCallerIdentityOutcomeCallable cb = client.getCallerIdentityCallable(req);
    StsClient::GetCallerIdentityOutcome out = cb.get();
    EXPECT_TRUE(out.error().errorCode().empty());
    EXPECT_TRUE(out.result().accountId() == "123456789");
    EXPECT_TRUE(out.result().userId() == "any-id");
    EXPECT_TRUE(out.result().arn() == "any-arn");
    ShutdownSdk();
  }

  void cb(const StsClient *client,
          const Model::GetCallerIdentityRequest &req,
          const StsClient::GetCallerIdentityOutcome& out,
          const std::shared_ptr<const AsyncCallerContext>& contex) {
    EXPECT_FALSE(out.error().errorCode().empty());
  }

  TEST(StsClient, getCallerIdentity_async) {
    InitializeSdk();
    ClientConfiguration configuration("cn-hangzhou");
    Model::GetCallerIdentityRequest req;
    StsClient client("key", "secret", configuration);
    const AsyncCallerContext context;

    StsClient::GetCallerIdentityAsyncHandler handler(cb);
    client.getCallerIdentityAsync(req, handler, std::make_shared<const AsyncCallerContext>(context));
    ShutdownSdk();
  }

  TEST(StsClient, assumeRole) {
    ClientConfiguration configuration;
    RpcServiceClient::JsonOutcome xout("{\"RequestId\":\"any-request-id\",\"success\":true,\"AssumedRoleUser\":{\"AssumedRoleId\":\"123456789\",\"Arn\":\"any-arn\"},\"Credentials\":{\"AccessKeyId\":\"any-key\",\"AccessKeySecret\":\"any-secret\",\"Expiration\":\"2019-01-15T16:31:10Z\",\"SecurityToken\":\"any-token\"}}");
    mockStsClient client("mock-key", "mock-secret", configuration);
    DefaultValue<RpcServiceClient::JsonOutcome>::Set(xout);
    EXPECT_CALL(client, makeRequest(_, _, _));
    Model::AssumeRoleRequest req;
    StsClient::AssumeRoleOutcome out = client.assumeRole(req);
    EXPECT_TRUE(out.error().errorCode().empty());
    EXPECT_TRUE(out.result().assumedRoleUser().arn == "any-arn");
    EXPECT_TRUE(out.result().assumedRoleUser().assumedRoleId == "123456789");
    EXPECT_TRUE(out.result().credentials().accessKeyId == "any-key");
    EXPECT_TRUE(out.result().credentials().accessKeySecret == "any-secret");
    EXPECT_TRUE(out.result().credentials().expiration == "2019-01-15T16:31:10Z");
    EXPECT_TRUE(out.result().credentials().securityToken == "any-token");
  }

  TEST(StsClient, assumeRole_callable) {
    InitializeSdk();
    ClientConfiguration configuration;
    RpcServiceClient::JsonOutcome xout("{\"RequestId\":\"any-request-id\",\"success\":true,\"AssumedRoleUser\":{\"AssumedRoleId\":\"123456789\",\"Arn\":\"any-arn\"},\"Credentials\":{\"AccessKeyId\":\"any-key\",\"AccessKeySecret\":\"any-secret\",\"Expiration\":\"2019-01-15T16:31:10Z\",\"SecurityToken\":\"any-token\"}}");
    mockStsClient client("mock-key", "mock-secret", configuration);
    DefaultValue<RpcServiceClient::JsonOutcome>::Set(xout);
    EXPECT_CALL(client, makeRequest(_, _, _));
    Model::AssumeRoleRequest req;
    StsClient::AssumeRoleOutcomeCallable cb = client.assumeRoleCallable(req);
    StsClient::AssumeRoleOutcome out = cb.get();

    EXPECT_TRUE(out.error().errorCode().empty());
    EXPECT_TRUE(out.result().assumedRoleUser().arn == "any-arn");
    EXPECT_TRUE(out.result().assumedRoleUser().assumedRoleId == "123456789");
    EXPECT_TRUE(out.result().credentials().accessKeyId == "any-key");
    EXPECT_TRUE(out.result().credentials().accessKeySecret == "any-secret");
    EXPECT_TRUE(out.result().credentials().expiration == "2019-01-15T16:31:10Z");
    EXPECT_TRUE(out.result().credentials().securityToken == "any-token");
    ShutdownSdk();
  }

  TEST(StsClient, assumeRole_error) {
    ClientConfiguration configuration;
    RpcServiceClient::JsonOutcome xout(Error("any-error-code", "any-error-message"));
    mockStsClient client("mock-key", "mock-secret", configuration);
    DefaultValue<RpcServiceClient::JsonOutcome>::Set(xout);
    EXPECT_CALL(client, makeRequest(_, _, _));
    Model::AssumeRoleRequest req;
    StsClient::AssumeRoleOutcome out = client.assumeRole(req);
    EXPECT_TRUE(out.error().errorCode() == "any-error-code");
  }

  void assumerole_cb(const StsClient *client,
          const Model::AssumeRoleRequest &req,
          const StsClient::AssumeRoleOutcome& out,
          const std::shared_ptr<const AsyncCallerContext>& contex) {
    EXPECT_FALSE(out.error().errorCode().empty());
  }

  TEST(StsClient, assumeRole_async) {
    InitializeSdk();
    ClientConfiguration configuration("cn-hangzhou");
    Model::AssumeRoleRequest req;
    req.setRoleArn("acs:ram::1940345800212929:root");
    req.setRoleSessionName("test_session");
    req.setDurationSeconds(1000);
    StsClient client("key", "secret", configuration);
    const AsyncCallerContext context;
    StsClient::AssumeRoleAsyncHandler handler(assumerole_cb);
    client.assumeRoleAsync(req, handler, std::make_shared<const AsyncCallerContext>(context));
    ShutdownSdk();
  }
}
