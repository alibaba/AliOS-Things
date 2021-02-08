#include "gtest/gtest.h"
#include "alibabacloud/core/Credentials.h"
#include "alibabacloud/core/sts/StsClient.h"
#include "alibabacloud/core/SimpleCredentialsProvider.h"

using namespace std;
using namespace AlibabaCloud;

TEST(SimpleCredentialsProvider, basic) {

  const string access_key = "accessKeyId";
  const string access_secret = "accessKeySecret";
  Credentials credentials(access_key, access_secret);

  SimpleCredentialsProvider p1(credentials);
  SimpleCredentialsProvider p2(access_key, access_secret);

  EXPECT_TRUE(p1.getCredentials().accessKeyId() == access_key);
  EXPECT_TRUE(p1.getCredentials().accessKeySecret() == access_secret);

  EXPECT_TRUE(p2.getCredentials().accessKeyId() == access_key);
  EXPECT_TRUE(p2.getCredentials().accessKeySecret() == access_secret);
}
