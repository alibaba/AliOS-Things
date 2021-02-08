#include <iostream>
#include "../utils.h"
#include "gtest/gtest.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/vpc/VpcClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Vpc;

namespace {
  TEST(vpc, describeNatGateways) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");
    InitializeSdk();
    // ClientConfiguration configuration("cn-hangzhou");
    // VpcClient client(key, secret, configuration);
    // Model::DescribeNatGatewaysRequest request;
    // request.setPageSize(10);
    // auto outcome = client.describeNatGateways(request);
    // EXPECT_TRUE(outcome.isSuccess());
    // EXPECT_TRUE(outcome.error().errorCode().empty());
    // EXPECT_TRUE(outcome.result().getTotalCount() >= 0);
    // EXPECT_NO_THROW();
    ShutdownSdk();
  }
}
