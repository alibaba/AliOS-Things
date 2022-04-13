#include "gtest/gtest.h"
#include "alibabacloud/core/CommonRequest.h"

using namespace std;
using namespace AlibabaCloud;

TEST(CommonRequest, basic) {
  CommonRequest cr;

  cr.setDomain("testDomain");
  cr.setHeaderParameter("name1", "value1");
  cr.setHttpMethod(HttpRequest::Method::Get);
  cr.setQueryParameter("name2", "value2");
  cr.setRequestPattern(CommonRequest::RequestPattern::RpcPattern);
  EXPECT_TRUE(cr.domain() == "testDomain");
  EXPECT_TRUE(cr.httpMethod() == HttpRequest::Method::Get);
  EXPECT_TRUE(cr.requestPattern() == CommonRequest::RequestPattern::RpcPattern);
  EXPECT_TRUE(cr.headerParameter("name1") == "value1");
  EXPECT_TRUE(cr.queryParameter("name2") == "value2");
  EXPECT_TRUE(cr.queryParameters()["name2"] == "value2");
  EXPECT_TRUE(cr.headerParameters()["name1"] == "value1");
}
