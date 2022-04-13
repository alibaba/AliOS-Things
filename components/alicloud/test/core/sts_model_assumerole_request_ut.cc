#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/sts/model/AssumeRoleRequest.h"

using namespace std;
using namespace AlibabaCloud;

TEST(AssumeRoleRequest, basic) {
  const string policy = "test_policy";
  const string role_arn = "test_role_arn";
  const string role_session_name = "test_role_session_name";
  Sts::Model::AssumeRoleRequest req;
  req.setDurationSeconds(100);
  req.setPolicy(policy);
  req.setRoleArn(role_arn);
  req.setRoleSessionName(role_session_name);
  EXPECT_TRUE(req.durationSeconds() == 100);
  EXPECT_TRUE(req.policy() == policy);
  EXPECT_TRUE(req.roleArn() == role_arn);
  EXPECT_TRUE(req.roleSessionName() == role_session_name);
}
