#include "gtest/gtest.h"
#include "alibabacloud/core/HttpResponse.h"

using namespace std;
using namespace AlibabaCloud;

TEST(HttpResponse, basic) {
  HttpResponse r;
  r.setStatusCode(202);
  EXPECT_TRUE(r.statusCode() == 202);

  Url url;
  url.setHost("example.com");
  HttpRequest req(url, HttpRequest::Method::Post);
  HttpResponse res(req);

  EXPECT_TRUE(res.request().url().host() == "example.com");
}
