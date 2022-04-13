#include "gtest/gtest.h"
#include "alibabacloud/core/NetworkProxy.h"

using namespace std;
using namespace AlibabaCloud;

TEST(NetworProxyTest, basic) {
  NetworkProxy proxy(NetworkProxy::Http);
  proxy.setHostName("abc.com");
  proxy.setPassword("myPassword");
  proxy.setPort(123);
  proxy.setUser("tester");

  EXPECT_TRUE(proxy.type() == NetworkProxy::Http);
  proxy.setType(NetworkProxy::Socks5);
  EXPECT_TRUE(proxy.type() == NetworkProxy::Socks5);
  EXPECT_TRUE(proxy.hostName() == "abc.com");
  EXPECT_TRUE(proxy.password() == "myPassword");
  EXPECT_TRUE(proxy.user() == "tester");
  EXPECT_TRUE(proxy.port() == 123);
}
