#include <iostream>
#include "../utils.h"
#include "gtest/gtest.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/slb/SlbClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Slb;

namespace {
  TEST(slb, describeLoadBalancers) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");
    InitializeSdk();
    // ClientConfiguration configuration("cn-hangzhou");
    // SlbClient client(key, secret, configuration);
    // Model::DescribeLoadBalancersRequest request;
    // request.setPageSize(10);
    // auto outcome = client.describeLoadBalancers(request);
    // EXPECT_TRUE(outcome.isSuccess());
    // EXPECT_TRUE(outcome.error().errorCode().empty());
    // EXPECT_TRUE(outcome.result().getTotalCount() >= 0);
    // EXPECT_NO_THROW();
    ShutdownSdk();
  }
}
