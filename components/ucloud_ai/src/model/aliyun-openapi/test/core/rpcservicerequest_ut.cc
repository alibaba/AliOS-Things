#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/RpcServiceRequest.h"

using namespace std;
using namespace AlibabaCloud;

TEST(test_RpcServiceRequest, basic) {
  const string product = "ECS";
  const string version = "1.0";
  const string action = "testAction";
  RpcServiceRequest rpc(product, version, action);

  EXPECT_TRUE(rpc.actionName() == action);
  EXPECT_TRUE(rpc.product() == product);
  EXPECT_TRUE(rpc.version() == version);
  EXPECT_EQ(string("testAction"), rpc.getHeader(string("x-acs-action")));
}
