#include "gtest/gtest.h"
#include "alibabacloud/core/Url.h"

using namespace std;
using namespace AlibabaCloud;

namespace {
  TEST(UrlTest, assign) {
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
    EXPECT_TRUE(url.toString() == src);

    EXPECT_TRUE(url.scheme() == "abc");
    EXPECT_TRUE(url.userName() == "username");
    EXPECT_TRUE(url.password() == "password");
    EXPECT_TRUE(url.host() == "example.com");
    EXPECT_TRUE(url.path() == "/path/data");
    EXPECT_TRUE(url.port() == 123);
    EXPECT_TRUE(url.query() == "key=value&key2=value2");
    EXPECT_TRUE(url.fragment() == "fragid1");

    url.clear();
    EXPECT_TRUE(url.toString() == "");
    EXPECT_TRUE(url.scheme() == "");
    EXPECT_TRUE(url.userName() == "");
    EXPECT_TRUE(url.password() == "");
    EXPECT_TRUE(url.host() == "");
    EXPECT_TRUE(url.path() == "");
    EXPECT_TRUE(url.port() == -1);
    EXPECT_TRUE(url.query() == "");
    EXPECT_TRUE(url.fragment() == "");

    url.fromString(src);

    EXPECT_TRUE(url.scheme() == "abc");
    EXPECT_TRUE(url.userName() == "username");
    EXPECT_TRUE(url.password() == "password");
    EXPECT_TRUE(url.host() == "example.com");
    EXPECT_TRUE(url.path() == "/path/data");
    EXPECT_TRUE(url.port() == 123);
    EXPECT_TRUE(url.query() == "key=value&key2=value2");
    EXPECT_TRUE(url.fragment() == "fragid1");

    Url newurl = url;
    EXPECT_TRUE(newurl.scheme() == "abc");
    EXPECT_TRUE(newurl.userName() == "username");
    EXPECT_TRUE(newurl.password() == "password");
    EXPECT_TRUE(newurl.host() == "example.com");
    EXPECT_TRUE(newurl.path() == "/path/data");
    EXPECT_TRUE(newurl.port() == 123);
    EXPECT_TRUE(newurl.query() == "key=value&key2=value2");
    EXPECT_TRUE(newurl.fragment() == "fragid1");
  }

  TEST(UrlTest, from_string) {
    const string src = "http://oss.example.com";
    Url url(src);
    EXPECT_TRUE(url.scheme() == "http");
    EXPECT_TRUE(url.userName() == "");
    EXPECT_TRUE(url.password() == "");
    EXPECT_TRUE(url.host() == "oss.example.com");
    EXPECT_TRUE(url.path() == "/");
    EXPECT_TRUE(url.port() == -1);
    EXPECT_TRUE(url.query() == "");
    EXPECT_TRUE(url.fragment() == "");
  }

  TEST(UrlTest, abnormal) {
    const string src = "http://oss.example.com";
    Url url(src);
    Url newurl = url;
    newurl.setPort(12345);
    EXPECT_TRUE(url.isEmpty() == false);
    EXPECT_TRUE(url.isValid());
    EXPECT_TRUE(newurl != url);
    EXPECT_TRUE(newurl.port() == 12345);
  }

  TEST(UrlTest, abnormal_2) {
    const string src = "";
    Url empty_url;
    empty_url.fromString(src);
    EXPECT_TRUE(empty_url.isEmpty());
    Url url(src);
    Url newurl = url;
    EXPECT_TRUE(url == newurl);

    newurl.setPort(12345);

    EXPECT_TRUE(url.isEmpty() == true);
    EXPECT_TRUE(url.isValid() == false);
    EXPECT_TRUE(url.authority() == "");
    EXPECT_TRUE(newurl != url);
    EXPECT_TRUE(newurl.port() == 12345);
    newurl.setAuthority("");
    EXPECT_TRUE(newurl.host() == "");
    EXPECT_TRUE(newurl.port() == -1);
    EXPECT_TRUE(newurl.userInfo() == "");

    newurl.setScheme("");
    EXPECT_TRUE(newurl.scheme() == "");

    newurl.setUserInfo("userinfo");
    EXPECT_TRUE(newurl.userName() == "userinfo");

    newurl.setUserInfo("user:password");
    EXPECT_TRUE(newurl.userName() == "user");
    EXPECT_TRUE(newurl.password() == "password");
  }
}
