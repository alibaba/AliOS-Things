#include "gtest/gtest.h"
#include "alibabacloud/core/ClientConfiguration.h"
#include "alibabacloud/core/AlibabaCloud.h"

using namespace std;
using namespace AlibabaCloud;


TEST(ClientConfiguration, basic) {
  const std::string regionId = "cn-shanghai";
  const std::string regionId_new = "cn-beijing";
  const std::string endpoint = "ep-cn-shanghai";

  const std::string hostname = "hostname";
  const std::string user = "user";
  const std::string password = "password";
  uint16_t port = 12345;
  const NetworkProxy proxy(NetworkProxy::Http, hostname, port, user, password);

  ClientConfiguration config(regionId);
  EXPECT_TRUE(config.regionId() == regionId);

  config.setEndpoint(endpoint);
  config.setProxy(proxy);
  config.setRegionId(regionId_new);
  EXPECT_TRUE(config.endpoint() == endpoint);
  EXPECT_TRUE(config.regionId() == regionId_new);
  EXPECT_TRUE(config.proxy().hostName() == hostname);
  EXPECT_TRUE(config.proxy().port() == port);
  EXPECT_TRUE(config.proxy().user() == user);
  EXPECT_TRUE(config.proxy().password() == password);

  EXPECT_TRUE(config.connectTimeout() == kDefaultConnectTimeout);
  EXPECT_TRUE(config.readTimeout() == kDefaultReadTimeout);
  config.setConnectTimeout(1222);
  config.setReadTimeout(23333);
  EXPECT_TRUE(config.connectTimeout() == 1222);
  EXPECT_TRUE(config.readTimeout() == 23333);
}
