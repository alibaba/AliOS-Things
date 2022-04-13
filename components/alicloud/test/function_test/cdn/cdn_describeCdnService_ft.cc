#include <iostream>
#include "../utils.h"
#include "gtest/gtest.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/cdn/CdnClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Cdn;

namespace
{
TEST(cdn, describeCdnService)
{
  utUtils utils;
  string key = utils.get_env("ENV_AccessKeyId");
  string secret = utils.get_env("ENV_AccessKeySecret");
  string uid = utils.get_env("ENV_TEST_UID");
  InitializeSdk();
  // ClientConfiguration configuration("cn-hangzhou");
  // CdnClient client(key, secret, configuration);
  // Model::DescribeCdnServiceRequest request;
  // CdnClient::DescribeCdnServiceOutcome outcome = client.describeCdnService(request);

  // EXPECT_NO_THROW();
  ShutdownSdk();
}
} // namespace
