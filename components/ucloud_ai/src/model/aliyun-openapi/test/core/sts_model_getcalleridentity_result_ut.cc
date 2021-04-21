#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/sts/model/GetCallerIdentityResult.h"

using namespace std;
using namespace AlibabaCloud;

TEST(GetCallerIdentityResult, basic) {
  const string payload = "{\"Request\":\"test-request-id\",\"AccountId\":\"test_account_id\",\"Arn\":\"test_arn\",\"UserId\":\"test_user_id\"}";
  Sts::Model::GetCallerIdentityResult res(payload);

  string arn = "test_arn";
  string account_id = "test_account_id";
  string user_id = "test_user_id";

  EXPECT_TRUE(res.accountId() == account_id);
  EXPECT_TRUE(res.arn() == arn);
  EXPECT_TRUE(res.userId() == user_id);
}

TEST(GetCallerIdentityResult, empty) {

  Sts::Model::GetCallerIdentityResult res;

  EXPECT_TRUE(res.accountId().empty());
  EXPECT_TRUE(res.arn().empty());
  EXPECT_TRUE(res.userId().empty());
}
