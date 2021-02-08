#include "gtest/gtest.h"
#include "alibabacloud/core/CommonResponse.h"


using namespace std;
using namespace AlibabaCloud;

TEST(CommonResponse, basic) {
  CommonResponse cres("test_payload_");
  EXPECT_TRUE(cres.payload() == "test_payload_");

  CommonResponse cres1;
  EXPECT_TRUE(cres1.payload() == "");
}
