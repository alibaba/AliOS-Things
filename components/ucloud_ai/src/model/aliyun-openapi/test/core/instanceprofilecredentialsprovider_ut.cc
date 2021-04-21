#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "alibabacloud/core/InstanceProfileCredentialsProvider.h"

using namespace std;
using namespace AlibabaCloud;

using ::testing::_;
using ::testing::DefaultValue;


class mockInstanceProfileCredentialsProvider: public InstanceProfileCredentialsProvider {
 public:
  mockInstanceProfileCredentialsProvider(const std::string &roleName, int durationSeconds):

  InstanceProfileCredentialsProvider(roleName, durationSeconds) {}
  MOCK_METHOD0(getMetadata, std::string());
};

TEST(InstanceProfileCredentialsProvider, basic) {
  mockInstanceProfileCredentialsProvider p("test-role-name", 1800);

  DefaultValue<std::string>::Set("{\"Code\":\"test-code\",\"AccessKeyId\":\"any-key\",\"AccessKeySecret\":\"any-secret\",\"SecurityToken\":\"any-token\",\"Expiration\":3233}");
  EXPECT_CALL(p, getMetadata());

  Credentials credentials = p.getCredentials();
  EXPECT_TRUE(credentials.accessKeyId() == "any-key");
  EXPECT_TRUE(credentials.accessKeySecret() == "any-secret");
  EXPECT_TRUE(credentials.sessionToken() == "any-token");
}

TEST(InstanceProfileCredentialsProvider, empty) {
  mockInstanceProfileCredentialsProvider p("test-role-name", 1800);

  DefaultValue<std::string>::Set("{}");
  EXPECT_CALL(p, getMetadata());

  Credentials credentials = p.getCredentials();
  EXPECT_TRUE(credentials.accessKeyId().empty());
  EXPECT_TRUE(credentials.accessKeySecret().empty());
  EXPECT_TRUE(credentials.sessionToken().empty());
}

TEST(InstanceProfileCredentialsProvider, empty_1) {
  mockInstanceProfileCredentialsProvider p("test-role-name", 1800);

  DefaultValue<std::string>::Set("");
  EXPECT_CALL(p, getMetadata());

  Credentials credentials = p.getCredentials();
  EXPECT_TRUE(credentials.accessKeyId().empty());
  EXPECT_TRUE(credentials.accessKeySecret().empty());
  EXPECT_TRUE(credentials.sessionToken().empty());
}