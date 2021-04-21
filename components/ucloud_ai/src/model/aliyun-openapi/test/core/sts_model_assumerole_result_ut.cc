#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/sts/model/AssumeRoleResult.h"

using namespace std;
using namespace AlibabaCloud;

TEST(AssumeRoleResult, basic) {
  const string payload = "{\"RequestId\":\"a-b-c-d\",\"AssumedRoleUser\":{\"AssumedRoleId\":\"test_assumed_role_id\",\"Arn\":\"test_arn\"},\"Credentials\":{\"AccessKeyId\":\"test_access_key_id\",\"AccessKeySecret\":\"test_access_key_secret\",\"Expiration\":\"100\",\"SecurityToken\":\"test_security_token\"}}";
  Sts::Model::AssumeRoleResult res(payload);

  string arn = "test_arn";
  string assumed_role_id = "test_assumed_role_id";

  string access_key_id = "test_access_key_id";
  string access_key_secret = "test_access_key_secret";
  string expiration = "100";
  string security_token = "test_security_token";

  EXPECT_TRUE(res.assumedRoleUser().arn == arn);
  EXPECT_TRUE(res.assumedRoleUser().assumedRoleId == assumed_role_id);

  EXPECT_TRUE(res.credentials().accessKeyId == access_key_id);
  EXPECT_TRUE(res.credentials().accessKeySecret == access_key_secret);
  EXPECT_TRUE(res.credentials().expiration == expiration);
  EXPECT_TRUE(res.credentials().securityToken == security_token);
}

TEST(AssumeRoleResult, empty) {

  Sts::Model::AssumeRoleResult res;

  EXPECT_TRUE(res.assumedRoleUser().arn.empty());
  EXPECT_TRUE(res.assumedRoleUser().assumedRoleId.empty());

  EXPECT_TRUE(res.credentials().accessKeyId.empty());
  EXPECT_TRUE(res.credentials().accessKeySecret.empty());
  EXPECT_TRUE(res.credentials().expiration.empty());
  EXPECT_TRUE(res.credentials().securityToken.empty());
}
