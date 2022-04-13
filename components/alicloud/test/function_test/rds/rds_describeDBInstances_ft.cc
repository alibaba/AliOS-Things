#include <iostream>
#include "../utils.h"
#include "gtest/gtest.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/rds/RdsClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Rds;

namespace {
  TEST(rds, describeDBInstances) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");
    InitializeSdk();
    // ClientConfiguration configuration("cn-hangzhou");
    // RdsClient client(key, secret, configuration);
    // Model::DescribeDBInstancesRequest request;
    // request.setPageSize(10);
    // auto outcome = client.describeDBInstances(request);
    // EXPECT_TRUE(outcome.isSuccess());
    // EXPECT_TRUE(outcome.error().errorCode().empty());
    // EXPECT_TRUE(outcome.result().getTotalRecordCount() >= 0);
    // EXPECT_NO_THROW();
    ShutdownSdk();
  }
}
