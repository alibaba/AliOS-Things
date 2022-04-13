#include "gtest/gtest.h"
#include "alibabacloud/core/Utils.h"
#include <stdlib.h>

using namespace std;
using namespace AlibabaCloud;

namespace {
  TEST(UtilsTest, ComputeContentMD5) {
    const std::string src = "abc://username:password@example.com:123/path/data?key=value&key2=value2#fragid1";
    const std::string md5 = ComputeContentMD5(src.c_str(), src.size());
    EXPECT_TRUE(md5 == "WL7+fwrxHSlBTsfU9WCSbA==");
  }

  TEST(UtilsTest, GenerateUuid) {
    const std::string uuid1 = GenerateUuid();
    const std::string uuid2 = GenerateUuid();
    EXPECT_TRUE(uuid1 != uuid2);
  }

  TEST(UtilsTest, HttpMethodToString){
    std::string method = HttpMethodToString(HttpRequest::Method::Head);
    EXPECT_TRUE(method == "HEAD");
    method = HttpMethodToString(HttpRequest::Method::Post);
    EXPECT_TRUE(method == "POST");
    method = HttpMethodToString(HttpRequest::Method::Put);
    EXPECT_TRUE(method == "PUT");
    method = HttpMethodToString(HttpRequest::Method::Delete);
    EXPECT_TRUE(method == "DELETE");
    method = HttpMethodToString(HttpRequest::Method::Connect);
    EXPECT_TRUE(method == "CONNECT");
    method = HttpMethodToString(HttpRequest::Method::Options);
    EXPECT_TRUE(method == "OPTIONS");
    method = HttpMethodToString(HttpRequest::Method::Patch);
    EXPECT_TRUE(method == "PATCH");
    method = HttpMethodToString(HttpRequest::Method::Trace);
    EXPECT_TRUE(method == "TRACE");
    method = HttpMethodToString(HttpRequest::Method::Get);
    EXPECT_TRUE(method == "GET");
  }

  TEST(UtilsTest, StringReplace) {
    std::string src = "abc://username:password@example.com:123/path/data?key=value&key2=value2#fragid1";
    StringReplace(src, "abc", "http");
    EXPECT_TRUE(src == "http://username:password@example.com:123/path/data?key=value&key2=value2#fragid1");
    StringReplace(src, "value", "VALUE");
    EXPECT_TRUE(src == "http://username:password@example.com:123/path/data?key=VALUE&key2=VALUE2#fragid1");
  }

  TEST(UtilsTest, Encode) {
    const std::string url = "http://username:password@example.com:123/path/data?key=value&key2=value2#fragid1";
    const std::string encoded = "http%3A%2F%2Fusername%3Apassword%40example.com%3A123%2Fpath%2Fdata%3Fkey%3Dvalue%26key2%3Dvalue2%23fragid1";
    const std::string enc = UrlEncode(url);
    EXPECT_TRUE(enc == encoded);
  }

  TEST(UtilsTest, Decode) {
    const std::string url = "http://username:password@example.com:123/path/data?key=value&key2=value2#fragid1";
    const std::string encoded = "http%3A%2F%2Fusername%3Apassword%40example.com%3A123%2Fpath%2Fdata%3Fkey%3Dvalue%26key2%3Dvalue2%23fragid1";
    const std::string decoded = UrlDecode(encoded);
    EXPECT_TRUE(decoded == url);
  }

  TEST(UtilsTest, MapToJson){
    std::map<std::string, std::string> maps;
    maps.insert(std::make_pair("foo", "bar"));
    std::string jsonStr = MapToJson(maps);
    EXPECT_EQ(jsonStr, "{\"foo\":\"bar\"}");
  }

  TEST(UtilsTest, JsonToMap){
    std::map<std::string, std::string> targetMaps;
    targetMaps.insert(std::make_pair("foo", "bar"));
    targetMaps.insert(std::make_pair("int", "1"));
    targetMaps.insert(std::make_pair("array","foo,bar"));
    std::string jsonStr = "{\"foo\":\"bar\",\"int\":1,\"array\":[\"foo\",\"bar\"]}";
    std::map<std::string, std::string> maps = JsonToMap(jsonStr);
    EXPECT_EQ(targetMaps, maps);
  }

  TEST(Utils, GetEnv) {
    const std::string var1 = GetEnv("PATH");
    EXPECT_FALSE(var1.empty());
    const std::string var2 = GetEnv("NOT_EXISTS_PATH");
    EXPECT_TRUE(var2.empty());
#ifndef _WIN32
    setenv("SDK", "ttt", 1);
    EXPECT_TRUE(GetEnv("SDK") == "ttt");
#endif
  }
}
