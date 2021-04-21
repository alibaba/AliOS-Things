#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/sts/StsClient.h"
#include "alibabacloud/core/StsAssumeRoleCredentialsProvider.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Sts;

using ::testing::_;
using ::testing::DefaultValue;

namespace {

  class mockStsAssumeRoleCredentialsProvider : public StsAssumeRoleCredentialsProvider {
   public:
    mockStsAssumeRoleCredentialsProvider(
      const Credentials &credentials,
      const ClientConfiguration &configuration,
      const std::string &roleArn,
      const std::string &roleSessionName,
      const std::string &policy = "",
      int durationSeconds = 3600):
    StsAssumeRoleCredentialsProvider(credentials, configuration, roleArn, roleSessionName, policy, durationSeconds) {}
    MOCK_CONST_METHOD1(assumeRole, StsClient::AssumeRoleOutcome(const Model::AssumeRoleRequest &request));
  };

  TEST(StsAssumeRoleCredentialsProvider, mock) {
    ClientConfiguration configuration("cn-hangzhou");
    const string sub_user_key = "key";
    const string sub_user_secret = "secret";

    const Credentials sub_user_credentials(sub_user_key, sub_user_secret);

    const string arn = "acs:ram::1940345800212929:role/testassumerole";
    const string session = "roleSessionName";
    const string policy = "";
    int duration = 1800;

    const string payload = "{\"RequestId\":\"a-b-c-d\",\"AssumedRoleUser\":{\"AssumedRoleId\":\"test_assumed_role_id\",\"Arn\":\"test_arn\"},\"Credentials\":{\"AccessKeyId\":\"test_access_key_id\",\"AccessKeySecret\":\"test_access_key_secret\",\"Expiration\":\"100\",\"SecurityToken\":\"test_security_token\"}}";
    Model::AssumeRoleResult res(payload);

    mockStsAssumeRoleCredentialsProvider provider(sub_user_credentials, configuration, arn, session, policy, duration);

    StsClient::AssumeRoleOutcome xout(res);
    DefaultValue<StsClient::AssumeRoleOutcome>::Set(xout);
    EXPECT_CALL(provider, assumeRole(_));

    Credentials assumed_role_credentials = provider.getCredentials();

    EXPECT_TRUE(assumed_role_credentials.accessKeyId() == "test_access_key_id");
    EXPECT_TRUE(assumed_role_credentials.accessKeySecret() == "test_access_key_secret");
    EXPECT_TRUE(assumed_role_credentials.sessionToken() == "test_security_token");
  }
}
