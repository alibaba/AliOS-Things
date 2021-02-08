
#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/Utils.h"
#include "alibabacloud/core/Config.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/CommonClient.h"
#include "alibabacloud/core/CommonResponse.h"
#include "alibabacloud/core/CommonRequest.h"
#include "alibabacloud/core/SimpleCredentialsProvider.h"


using namespace std;
using namespace AlibabaCloud;

namespace AlibabaCloud {
  class TestCommonClient : public CommonClient {
    public:
      TestCommonClient(const Credentials &credentials, const ClientConfiguration &configuration):
        CommonClient(credentials, configuration)
      {}
      using CommonClient::buildHttpRequest;
      using CommonClient::buildRoaHttpRequest;
      using CommonClient::buildRpcHttpRequest;
      using CommonClient::makeRequest;
      using CommonClient::asyncExecute;
      using CommonClient::commonResponseAsync;
  };
}

static int nbr = 0;
static void task() {
   nbr |= 0x01;
}

namespace {

  TEST(CommonClient, basic) {
    InitializeSdk();
    const ClientConfiguration cfg;

    std::string key    = "accessKeyId";
    std::string secret = "accessSecret";
    std::string token  = "sessionToken";

    const Credentials credentials(key, secret, token);

    const CommonClient cc1(credentials, cfg);
    CommonClient cc2(std::make_shared<SimpleCredentialsProvider>(credentials), cfg);
    CommonClient cc3(key, secret, cfg);
    CommonClient cc4(key, secret, cfg);

    CommonRequest cr;
    cr.setContent("test-content", 12);

    CommonClient::CommonResponseOutcome out1 = cc1.commonResponse(cr);
    EXPECT_TRUE(out1.error().errorCode() == "NetworkError");
    EXPECT_TRUE(out1.error().errorMessage().find("Failed to connect to host or proxy: GET ") == 0);

    EXPECT_TRUE(out1.result().payload() == "");

    CommonClient::CommonResponseOutcome out2 = cc2.commonResponse(cr);
    EXPECT_TRUE(out2.error().errorCode() == "NetworkError");
    EXPECT_TRUE(out2.error().errorMessage().find("Failed to connect to host or proxy: GET ") == 0);
    EXPECT_TRUE(out2.result().payload() == "");

    CommonClient::CommonResponseOutcome out3 = cc3.commonResponse(cr);
    EXPECT_TRUE(out3.error().errorCode() == "NetworkError");
    EXPECT_TRUE(out3.error().errorMessage().find("Failed to connect to host or proxy: GET ") == 0);
    EXPECT_TRUE(out3.result().payload() == "");

    cr.setRequestPattern(CommonRequest::RequestPattern::RoaPattern);
    CommonClient::CommonResponseOutcome out4 = cc4.commonResponse(cr);
    EXPECT_TRUE(out4.error().errorCode() == "NetworkError");
    EXPECT_TRUE(out4.error().errorMessage().find("Failed to connect to host or proxy: GET ") == 0);
    EXPECT_TRUE(out4.result().payload() == "");

  /*
  https://cn-hangzhou/?
     AccessKeyId=accessKeyId
    &Format=JSON
    &RegionId=cn-hangzhou
    &SecurityToken=sessionToken
    &Signature=tWGagYMK93km44TOY%2FY60snVYWE%3D
    &SignatureMethod=HMAC-SHA1
    &SignatureNonce=ef27311a-5151-40a0-974f-2b808a8a683e
    &SignatureVersion=1.0
    &Timestamp=2019-01-02T08%3A56%3A26Z
    &Version=
  */
    cr.setRequestPattern(CommonRequest::RequestPattern::RpcPattern);
    TestCommonClient* client = new TestCommonClient(credentials, cfg);
    HttpRequest r = client->buildHttpRequest("cn-hangzhou", cr, HttpRequest::Method::Post);
    EXPECT_TRUE(r.method() == HttpRequest::Method::Post);
    EXPECT_TRUE(r.url().scheme() == "https");
    EXPECT_TRUE(r.url().userName() == "");
    EXPECT_TRUE(r.url().password() == "");
    EXPECT_TRUE(r.url().host() == "cn-hangzhou");
    EXPECT_TRUE(r.url().port() == -1);
    EXPECT_TRUE(r.url().fragment() == "");
    EXPECT_TRUE(r.url().query().find("AccessKeyId=") != string::npos);
    EXPECT_TRUE(r.url().query().find("Format=") != string::npos);
    EXPECT_TRUE(r.url().query().find("RegionId=") != string::npos);
    EXPECT_TRUE(r.url().query().find("SecurityToken=") != string::npos);
    EXPECT_TRUE(r.url().query().find("Signature=") != string::npos);
    EXPECT_TRUE(r.url().query().find("SignatureMethod=") != string::npos);
    EXPECT_TRUE(r.url().query().find("SignatureNonce=") != string::npos);
    EXPECT_TRUE(r.url().query().find("SignatureVersion=") != string::npos);
    EXPECT_TRUE(r.url().query().find("Timestamp=") != string::npos);
    EXPECT_TRUE(r.url().query().find("Version=") != string::npos);

    cr.setQueryParameter("query_k1", "query_v1");
    cr.setHeaderParameter("header_k1", "header_v1");
    cr.setScheme("");
    cr.setHeaderParameter("Accept", "");
    HttpRequest rr = client->buildRoaHttpRequest("cn-shanghai", cr, HttpRequest::Method::Get);
    EXPECT_TRUE(rr.method() == HttpRequest::Method::Get);
    EXPECT_TRUE(rr.header("Accept") == "application/json");
    EXPECT_TRUE(rr.url().toString() == "https://cn-shanghai/?header_k1=header_v1");
    EXPECT_TRUE(rr.header("Host") == "cn-shanghai");
    // Wed, 09 Jan 2019 06:32:41 GMT
    string date = "Wed, 09 Jan 2019 06:32:41 GMT";
    EXPECT_TRUE(rr.header("Date").length() == date.length());
    string signature_nonce = "fb62c51d-f735-4ec7-870d-c6ce03368214";
    EXPECT_TRUE(rr.header("x-acs-signature-nonce").length() == signature_nonce.length());

    EXPECT_TRUE(rr.header("x-acs-signature-method") == "HMAC-SHA1");
    EXPECT_TRUE(rr.header("x-acs-signature-version") == "1.0");
    EXPECT_TRUE(rr.header("x-acs-version") == "");
    EXPECT_TRUE(rr.header("x-sdk-client") == string("CPP/").append(ALIBABACLOUD_VERSION_STR));
    EXPECT_TRUE(rr.header("x-acs-region-id") == "cn-hangzhou");
    // acs accessKeyId:JZD81jGWLp1F3ZIkaLp1yuEZmKc=
    EXPECT_TRUE(rr.header("Authorization").find("acs accessKeyId:") != string::npos);
    EXPECT_TRUE(rr.header("unknown-header") == "");

    cr.setScheme("http");
    cr.setHeaderParameter("Accept", "test-accept");
    cr.setHeaderParameter("Content-Type", "test-content-type");
    rr = client->buildRoaHttpRequest("cn-shanghai", cr, HttpRequest::Method::Get);
    EXPECT_TRUE(rr.url().toString() == "http://cn-shanghai/?Accept=test-accept&Content-Type=test-content-type&header_k1=header_v1");
    EXPECT_TRUE(rr.header("Accept") == "test-accept");
    EXPECT_TRUE(rr.header("Content-Type") == "test-content-type");

    HttpRequest rrr = client->buildRpcHttpRequest("cn-hangzhou", cr, HttpRequest::Method::Post);
    EXPECT_TRUE(client->serviceName() == "Common");
    EXPECT_TRUE(rrr.url().scheme() == "http");

    cr.setScheme("");
    rrr = client->buildRpcHttpRequest("cn-hangzhou", cr, HttpRequest::Method::Post);
    EXPECT_TRUE(rrr.url().scheme() == "https");

    std::function<void()> func(task);
    Runnable* rf = new Runnable(func);
    EXPECT_TRUE(nbr == 0);
    client->asyncExecute(rf);
    usleep(10000);
    EXPECT_EQ(nbr, 1);
    ShutdownSdk();
  }

  TEST(CommonClient, bodyParam) {
    const ClientConfiguration cfg;

    std::string key    = "accessKeyId";
    std::string secret = "accessSecret";
    std::string token  = "sessionToken";

    const Credentials credentials(key, secret, token);
    CommonRequest cr;
    cr.setBodyParameter("TestBodyParamKey", "TestBodyParamValue");
    TestCommonClient* client = new TestCommonClient(credentials, cfg);
    HttpRequest r = client->buildRpcHttpRequest("cn-hangzhou", cr, HttpRequest::Method::Post);
    const char *s = "TestBodyParamKey=TestBodyParamValue";
    EXPECT_EQ(*r.body(), *s);
  }

  TEST(CommonClient, abnormal) {
    InitializeSdk();

    ClientConfiguration configuration("cn-hangzhou");
    CommonClient client("key", "secret", configuration);

    CommonRequest req;
    req.setScheme("http");
    req.setDomain("aliyun.com");

    auto out = client.commonResponse(req);
    EXPECT_TRUE(out.error().errorCode() == "InvalidResponse");
    EXPECT_TRUE(out.result().payload().empty());
/*
1: =====================<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
1: <html>
1: <head><meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
1: <title>301 Moved Permanently</title></head>
1: <body bgcolor="white">
1: <h1>301 Moved Permanently</h1>
1: <p>The requested resource has been assigned a new permanent URI.</p>
1: <hr/>Powered by Tengine/Aserver</body>
1: </html>
*/
    EXPECT_FALSE(out.error().errorMessage().empty());
    ShutdownSdk();
  }

  void cb(const CommonClient *client,
          const CommonRequest &req,
          const CommonClient::CommonResponseOutcome& out,
          const std::shared_ptr<const AsyncCallerContext>& contex) {
    EXPECT_FALSE(out.error().errorCode().empty());
  }

  TEST(CommonClient, async) {
    InitializeSdk();
    const ClientConfiguration cfg;

    std::string key    = "accessKeyId";
    std::string secret = "accessSecret";
    std::string token  = "sessionToken";

    const Credentials credentials(key, secret, token);

    const CommonClient cc1(credentials, cfg);

    CommonRequest cr;
    cr.setContent("test-content", 12);

    TestCommonClient::CommonResponseAsyncHandler handler(cb);


    const AsyncCallerContext context;
    cc1.commonResponseAsync(cr, handler, std::make_shared<const AsyncCallerContext>(context));

    ShutdownSdk();
  }

}