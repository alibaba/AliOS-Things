#include <iostream>
#include <stdio.h>

#include "gtest/gtest.h"
#include "alibabacloud/core/HttpRequest.h"

using namespace std;
using namespace AlibabaCloud;

TEST(HttpMessage, perRequest) {
	const std::string data = "Thu, 19 Oct 2017 04:38:27 GMT";
	HttpRequest req;
	req.setHeader("accept", "application/xml");
  EXPECT_TRUE(req.header("accept") == "application/xml");
	req.setHeader("Accept", "application/json");
  EXPECT_TRUE(req.header("accept") == "application/json");
	EXPECT_TRUE(req.headers().size() == 1);
  req.setHeader("Date", data);
  EXPECT_TRUE(req.headers().size() == 2);
	EXPECT_TRUE(req.header(HttpMessage::KnownHeader::Date) == data);

  req.addHeader("header1", "value1");
  EXPECT_TRUE(req.header("header1") == "value1");
  EXPECT_TRUE(req.headers().size() == 3);

  req.addHeader(HttpMessage::KnownHeader::ContentMD5, "testmd5sume");
  EXPECT_TRUE(req.header(HttpMessage::KnownHeader::ContentMD5) == "testmd5sume");
  EXPECT_TRUE(req.headers().size() == 4);

  req.removeHeader("header1");
  EXPECT_TRUE(req.header("header1") == "");
  EXPECT_TRUE(req.headers().size() == 3);
  req.removeHeader(HttpMessage::KnownHeader::ContentMD5);
  EXPECT_TRUE(req.header(HttpMessage::KnownHeader::ContentMD5) == "");
  EXPECT_TRUE(req.headers().size() == 2);

  string body = "this is a test body";

  req.setBody(body.c_str(), body.size());
  EXPECT_TRUE(req.bodySize() == body.size());
  EXPECT_TRUE(req.hasBody());
  EXPECT_TRUE(req.body() != nullptr);
}
