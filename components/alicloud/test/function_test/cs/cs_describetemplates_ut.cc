#include <iostream>
#include "../utils.h"
#include "gtest/gtest.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/cs/CSClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::CS;

namespace {
  TEST(cs, DescribeClustersClientTimeout) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    InitializeSdk();
    ClientConfiguration configuration("cn-hangzhou");
    configuration.setConnectTimeout(1);
    configuration.setReadTimeout(123);

    CSClient client(key, secret, configuration);
    Model::DescribeClustersRequest request;
    auto outcome = client.describeClusters(request);
    EXPECT_TRUE(outcome.error().errorCode() == "OperationTimeoutError");
    EXPECT_TRUE(outcome.error().errorMessage().find("Timeout (connectTimeout: 1ms, readTimeout: 123ms) when connect or read") == 0);
    ShutdownSdk();
  }

  TEST(cs, DescribeClustersRequstTimeout) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    InitializeSdk();
    ClientConfiguration configuration("cn-hangzhou");
    CSClient client(key, secret, configuration);
    Model::DescribeClustersRequest request;
    request.setConnectTimeout(11);
    request.setReadTimeout(213);
    auto outcome = client.describeClusters(request);
    EXPECT_TRUE(outcome.error().errorCode() == "OperationTimeoutError");
    EXPECT_TRUE(outcome.error().errorMessage().find("Timeout (connectTimeout: 11ms, readTimeout: 213ms) when connect or read") == 0);
    ShutdownSdk();
  }

  TEST(cs, DescribeClustersClientRequstTimeout) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    InitializeSdk();
    ClientConfiguration configuration("cn-hangzhou");
    configuration.setConnectTimeout(1);
    configuration.setReadTimeout(123);

    CSClient client(key, secret, configuration);
    Model::DescribeClustersRequest request;
    request.setConnectTimeout(11);
    request.setReadTimeout(213);
    auto outcome = client.describeClusters(request);
    EXPECT_TRUE(outcome.error().errorCode() == "OperationTimeoutError");
    EXPECT_TRUE(outcome.error().errorMessage().find("Timeout (connectTimeout: 11ms, readTimeout: 213ms) when connect or read") == 0);
    ShutdownSdk();
  }
}
