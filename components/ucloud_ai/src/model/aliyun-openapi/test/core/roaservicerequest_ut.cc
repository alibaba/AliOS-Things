#include "gtest/gtest.h"
#include "alibabacloud/core/RoaServiceRequest.h"


using namespace std;
using namespace AlibabaCloud;

TEST(test_RoaServiceRequest, basic) {
  const string product = "ECS";
  const string version = "1.0";

  RoaServiceRequest roa(product, version);
  EXPECT_TRUE(roa.version() == version);
  EXPECT_TRUE(roa.product() == product);
}

