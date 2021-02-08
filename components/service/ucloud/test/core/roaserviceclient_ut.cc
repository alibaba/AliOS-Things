#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/SimpleCredentialsProvider.h"
#include "alibabacloud/core/RoaServiceClient.h"
#include "alibabacloud/core/HttpMessage.h"
#include "alibabacloud/core/HttpRequest.h"

using namespace std;
using namespace AlibabaCloud;

namespace AlibabaCloud {
  class TestRoaClient : public RoaServiceClient {
   public:

    TestRoaClient(const std::string & servicename, const std::shared_ptr<CredentialsProvider> &credentialsProvider,
      const ClientConfiguration &configuration,
      const std::shared_ptr<Signer> &signer = std::make_shared<HmacSha1Signer>()):
    RoaServiceClient(servicename, credentialsProvider, configuration, signer)
    {}

    JsonOutcome makeRequest(const std::string &endpoint, const RoaServiceRequest &msg, HttpRequest::Method method = HttpRequest::Method::Get)const {
      return RoaServiceClient::makeRequest(endpoint, msg, method);
    }

    HttpRequest buildHttpRequest(const std::string & endpoint, const ServiceRequest &msg, HttpRequest::Method method)const {
      return RoaServiceClient::buildHttpRequest(endpoint, msg, method);
    }
  };

  class TestRoaServiceRequest: public ServiceRequest {
   public:
    TestRoaServiceRequest(const std::string &product, const std::string &version):
    ServiceRequest(product, version){}

    using ServiceRequest::addParameter;
    using ServiceRequest::parameter;
    using ServiceRequest::removeParameter;
    using ServiceRequest::setContent;
    using ServiceRequest::setParameter;
    using ServiceRequest::setParameters;
    using ServiceRequest::setResourcePath;
    using ServiceRequest::setProduct;
    using ServiceRequest::setVersion;
  };
}

TEST(RoaServiceClient, basic) {
  const ClientConfiguration config;

  std::string key    = "accessKeyId";
  std::string secret = "accessSecret";
  std::string token  = "sessionToken";

  const Credentials credentials(key, secret, token);

  TestRoaClient client("ecs", std::make_shared<SimpleCredentialsProvider>(credentials), config);

  const string product = "ECS";
  const string version = "1.0";

  RoaServiceRequest roa_req(product, version);

  RoaServiceClient::JsonOutcome out = client.makeRequest("cn-shanghai", roa_req, HttpRequest::Method::Get);

  EXPECT_TRUE(out.error().errorCode() == "NetworkError");

  RoaServiceRequest req(product, version);

  req.setParameter("a", "b");
  req.setScheme("");
  req.setContent("123456789", 9);
  HttpRequest http_req = client.buildHttpRequest("cn-shanghai", req, HttpRequest::Method::Get);

  EXPECT_TRUE(http_req.url().toString() == "https://cn-shanghai/?a=b");
  EXPECT_TRUE(http_req.header("Accept") == "application/json");
  EXPECT_TRUE(http_req.header("Content-Length") == "9");
  EXPECT_TRUE(http_req.header("Content-Type") == "application/octet-stream");
  EXPECT_TRUE(http_req.header("Content-MD5") == "JfnnlDI7RTiF9RgfG2JNCw==");
  const string date = "Fri, 11 Jan 2019 14:08:39 GMT";
  EXPECT_TRUE(http_req.header("Date").length() == date.length());
  EXPECT_TRUE(http_req.header("Host") == "cn-shanghai");
  EXPECT_TRUE(http_req.header("x-sdk-cient") == "");
  EXPECT_TRUE(http_req.header("x-acs-region-id") == "cn-hangzhou");
  EXPECT_TRUE(http_req.header("x-acs-security-token") == token);
  EXPECT_TRUE(http_req.header("x-acs-signature-method") == "HMAC-SHA1");
  EXPECT_TRUE(http_req.header("x-acs-signature-version") == "1.0");
  EXPECT_TRUE(http_req.header("x-acs-version") == "1.0");
  const string nounce = "8a013b14-7bac-4652-8b5f-c02c8924e4ae";
  EXPECT_TRUE(http_req.header("x-acs-signature-nonce").length() == nounce.length());
  EXPECT_TRUE(http_req.header("Authorization").find("acs accessKeyId:") != string::npos);

  req.setScheme("http");
  req.setParameter("Accept", "tets-accept");
  req.setParameter("Content-Type", "test-content-type");
  http_req = client.buildHttpRequest("cn-shanghai", req, HttpRequest::Method::Get);
  EXPECT_TRUE(http_req.url().toString() == "http://cn-shanghai/?Accept=tets-accept&Content-Type=test-content-type&a=b");
}
