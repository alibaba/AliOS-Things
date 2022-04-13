
#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/CoreClient.h"

using namespace std;
using namespace AlibabaCloud;

namespace AlibabaCloud {
  class TestCoreClient : public CoreClient {
    public:
      TestCoreClient(const std::string & servicename, const ClientConfiguration &configuration):
        CoreClient(servicename, configuration)
      {}

      using CoreClient::buildCoreError;
      using CoreClient::hasResponseError;
      HttpRequest buildHttpRequest(const std::string & endpoint, const ServiceRequest &msg, HttpRequest::Method method)const {
        Url url;
        url.setScheme("abc");
        url.setUserName("username");
        HttpRequest req(url, HttpRequest::Method::Post);
        return req;
      }
  };
}


TEST(CoreClient, basic) {
  ClientConfiguration configuration;
  TestCoreClient client("test-service", configuration);

  HttpResponse res;
  Error e1 = client.buildCoreError(res);
  EXPECT_TRUE(e1.errorCode() == ("InvalidResponse"));
  EXPECT_TRUE(e1.errorMessage() == ("body is empty"));
  string body = "{\"Code\":\"any-error-code\",\"Message\":\"any-error-message\",\"HostId\":\"any-host-id\",\"RequestId\":\"any-request-id\"}";
  res.setBody(body.c_str(), body.size());
  Error e2 = client.buildCoreError(res);
  EXPECT_TRUE(e2.errorCode() == ("any-error-code"));
  EXPECT_TRUE(e2.errorMessage() == ("any-error-message"));
  EXPECT_TRUE(e2.host() == ("any-host-id"));
  EXPECT_TRUE(e2.requestId() == ("any-request-id"));
}
