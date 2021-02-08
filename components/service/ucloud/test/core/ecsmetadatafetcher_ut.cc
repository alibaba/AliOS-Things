#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "alibabacloud/core/Url.h"
#include "alibabacloud/core/HttpRequest.h"
#include "alibabacloud/core/HttpClient.h"
#include "../src/EcsMetadataFetcher.h"

using namespace std;
using namespace AlibabaCloud;

namespace {

  TEST(EcsMetadataFetcher, basic) {
    EcsMetadataFetcher fetcher;
    fetcher.setRoleName("test-role-name");
    EXPECT_TRUE(fetcher.roleName() == "test-role-name");
  }

  TEST(EcsMetadataFetcher, mock) {
    HttpRequest request;

    string body = "1234567";
    HttpResponse response(request);
    response.setStatusCode(200);
    response.setBody(body.c_str(), body.size());

    EcsMetadataFetcher fetcher;
    string out = fetcher.getMetadata("a", "b");
    EXPECT_TRUE(out == "NetworkError");
  }
}
