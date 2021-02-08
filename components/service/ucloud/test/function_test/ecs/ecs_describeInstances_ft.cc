#include <iostream>
#include "../utils.h"
#include "gtest/gtest.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/ecs/EcsClient.h"
#include "alibabacloud/core/CommonClient.h"
#include "alibabacloud/core/CommonRequest.h"
#include "alibabacloud/core/sts/StsClient.h"
#include "alibabacloud/core/StsAssumeRoleCredentialsProvider.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Ecs;

namespace {
  TEST(ecs, describeInstances) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    InitializeSdk();
    // ClientConfiguration configuration("cn-hangzhou");
    // EcsClient client(key, secret, configuration);
    // Model::DescribeInstancesRequest request;
    // request.setPageSize(10);
    // auto outcome = client.describeInstances(request);
    // EXPECT_TRUE(outcome.isSuccess());
    // EXPECT_TRUE(outcome.error().errorCode().empty());
    // EXPECT_TRUE(outcome.result().getTotalCount() >= 0);
    ShutdownSdk();
  }

  TEST(ecs, deleteInstance) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    InitializeSdk();
    // ClientConfiguration configuration("cn-hangzhou");
    // EcsClient client(key, secret, configuration);

    // Model::DeleteInstanceRequest delReq;

    // delReq.setInstanceId("non-exist-ecs-instance");
    // delReq.setForce(true);

    // auto outcome = client.deleteInstance(delReq);
    // EXPECT_TRUE(outcome.error().errorCode() == "InvalidInstanceId.NotFound");
    // EXPECT_TRUE(outcome.error().errorMessage() == "The specified InstanceId does not exist.");
    ShutdownSdk();
  }

  TEST(ecs, describeInstancesViaCommonRequest) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");
    InitializeSdk();

    // ClientConfiguration configuration("cn-hangzhou");
    // CommonClient client(key, secret, configuration);
    // CommonRequest request(CommonRequest::RpcPattern);

    // request.setScheme("https");
    // request.setDomain("ecs.aliyuncs.com");
    // request.setHttpMethod(HttpRequest::Get);
    // request.setHeaderParameter("Content-Type", "application/json;chrset=utf-8");
    // request.setVersion("2014-05-26");
    // request.setQueryParameter("action", "DescribeInstances");

    // auto outcome = client.commonResponse(request);

    // EXPECT_TRUE(outcome.error().errorCode().empty());
    // EXPECT_TRUE(outcome.result().payload().find("\"TotalCount\":") != string::npos);
    ShutdownSdk();
  }

  TEST(ecs, describeInstancesViaCommonRequestAndStsAssumeRole) {

    utUtils utils;
    const string sub_user_key = utils.get_env("ENV_AccessKeyId_AssumeRole");
    const string sub_user_secret = utils.get_env("ENV_AccessKeySecret_AssumeRole");
    const Credentials sub_user_credentials(sub_user_key, sub_user_secret);

    const string arn = "acs:ram::1940345800212929:role/testassumerole";
    const string session = "roleSessionName";
    const string policy = "";
    int duration = 1800;

    InitializeSdk();
    // ClientConfiguration configuration("cn-hangzhou");
    // StsAssumeRoleCredentialsProvider provider(sub_user_credentials, configuration, arn, session, policy, duration);
    // Credentials assumed_role_credentials = provider.getCredentials();
    // CommonClient client(assumed_role_credentials, configuration);
    // CommonRequest request(CommonRequest::RpcPattern);

    // request.setScheme("https");
    // request.setDomain("ecs.aliyuncs.com");
    // request.setHttpMethod(HttpRequest::Get);
    // request.setHeaderParameter("Content-Type", "application/json;chrset=utf-8");
    // request.setVersion("2014-05-26");
    // request.setQueryParameter("action", "DescribeInstances");

    // auto outcome = client.commonResponse(request);

    // EXPECT_TRUE(outcome.error().errorCode().empty());
    // EXPECT_TRUE(outcome.result().payload().find("\"TotalCount\":") != string::npos);
    // EXPECT_TRUE(outcome.result().payload().find("\"PageNumber\":") != string::npos);
    // EXPECT_TRUE(outcome.result().payload().find("\"PageSize\":") != string::npos);
    // EXPECT_TRUE(outcome.result().payload().find("\"Instances\":") != string::npos);
    ShutdownSdk();
  }


  TEST(ecs, describeInstancesClientTimeout) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    InitializeSdk();
    // ClientConfiguration configuration("cn-hangzhou");
    // configuration.setConnectTimeout(1);
    // configuration.setReadTimeout(123);
    // EcsClient client(key, secret, configuration);
    // Model::DescribeInstancesRequest request;
    // request.setPageSize(10);

    // auto outcome = client.describeInstances(request);
    // EXPECT_TRUE(outcome.error().errorCode() == "OperationTimeoutError");
    // EXPECT_TRUE(outcome.error().errorMessage().find("Timeout (connectTimeout: 1ms, readTimeout: 123ms) when connect or read") == 0);
    ShutdownSdk();
  }

  TEST(ecs, describeInstancesRequestTimeout) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    InitializeSdk();
    // ClientConfiguration configuration("cn-hangzhou");
    // EcsClient client(key, secret, configuration);
    // Model::DescribeInstancesRequest request;
    // request.setPageSize(10);
    // request.setConnectTimeout(11);
    // request.setReadTimeout(213);
    // auto outcome = client.describeInstances(request);
    // EXPECT_TRUE(outcome.error().errorCode() == "OperationTimeoutError");
    // EXPECT_TRUE(outcome.error().errorMessage().find("Timeout (connectTimeout: 11ms, readTimeout: 213ms) when connect or read") == 0);
    ShutdownSdk();
  }


  TEST(ecs, describeInstancesClientRequestTimeout) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    InitializeSdk();
    // ClientConfiguration configuration("cn-hangzhou");
    // configuration.setConnectTimeout(1);
    // configuration.setReadTimeout(123);
    // EcsClient client(key, secret, configuration);
    // Model::DescribeInstancesRequest request;
    // request.setPageSize(10);
    // request.setConnectTimeout(11);
    // request.setReadTimeout(213);
    // auto outcome = client.describeInstances(request);
    // EXPECT_TRUE(outcome.error().errorCode() == "OperationTimeoutError");
    // EXPECT_TRUE(outcome.error().errorMessage().find("Timeout (connectTimeout: 11ms, readTimeout: 213ms) when connect or read") == 0);
    ShutdownSdk();
  }
}
