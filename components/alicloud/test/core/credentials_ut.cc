#include "gtest/gtest.h"
#include "alibabacloud/core/Credentials.h"

using namespace std;
using namespace AlibabaCloud;

TEST(CredentialsTest, basic) {
  const string access_key = "accessKeyId";
  const string access_secret = "accessKeySecret";
  const string session_token = "sessionToken";
  const string new_access_key = "new_accessKeyId";
  const string new_access_secret = "new_accessKeySecret";
  const string new_session_token = "new_sessionToken";

  Credentials credentials(access_key, access_secret, session_token);

  EXPECT_TRUE(credentials.accessKeyId() == access_key);
  EXPECT_TRUE(credentials.accessKeySecret() == access_secret);
  EXPECT_TRUE(credentials.sessionToken() == session_token);

  credentials.setAccessKeyId(new_access_key);
  credentials.setAccessKeySecret(new_access_secret);
  credentials.setSessionToken(new_session_token);

  EXPECT_TRUE(credentials.accessKeyId() == new_access_key);
  EXPECT_TRUE(credentials.accessKeySecret() == new_access_secret);
  EXPECT_TRUE(credentials.sessionToken() == new_session_token);
}
