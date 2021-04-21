#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/HttpRequest.h"

using namespace std;
using namespace AlibabaCloud;

TEST(HttpRequest, basic) {
  const string src = "abc://username:password@example.com:123/path/data?key=value&key2=value2#fragid1";
  Url url;
  url.setScheme("abc");
  url.setUserName("username");
  url.setPassword("password");
  url.setHost("example.com");
  url.setPath("/path/data");
  url.setPort(123);
  url.setQuery("key=value&key2=value2");
  url.setFragment("fragid1");

  HttpRequest req(url, HttpRequest::Method::Post);

  EXPECT_TRUE(req.method() == HttpRequest::Method::Post);
  req.setMethod(HttpRequest::Method::Get);
  EXPECT_TRUE(req.method() == HttpRequest::Method::Get);

  req.setUrl(url);
  Url rUrl = req.url();
  EXPECT_TRUE(rUrl.toString() == src);

  EXPECT_TRUE(req.connectTimeout() == 5000);
  EXPECT_TRUE(req.readTimeout() == 10000);
  req.setConnectTimeout(12345);
  req.setReadTimeout(3335);
  EXPECT_TRUE(req.connectTimeout() == 12345);
  EXPECT_TRUE(req.readTimeout() == 3335);
}
