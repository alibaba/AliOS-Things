#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/sts/model/GetCallerIdentityRequest.h"

using namespace std;
using namespace AlibabaCloud;

TEST(GetCallerIdentityRequest, basic) {
  Sts::Model::GetCallerIdentityRequest req;
  EXPECT_TRUE(req.actionName() == "GetCallerIdentity");
}
