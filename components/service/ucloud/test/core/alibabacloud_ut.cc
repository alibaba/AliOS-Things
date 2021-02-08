#include "gtest/gtest.h"
#include "alibabacloud/core/AlibabaCloud.h"

using namespace std;
using namespace AlibabaCloud;


TEST(AlibabaCloudTest, basic) {
  InitializeSdk();
  EXPECT_TRUE(IsSdkInitialized() == true);
  InitializeSdk();
  EXPECT_TRUE(IsSdkInitialized() == true);

  ShutdownSdk();
  EXPECT_TRUE(IsSdkInitialized() == false);
  ShutdownSdk();
  EXPECT_TRUE(IsSdkInitialized() == false);
}
