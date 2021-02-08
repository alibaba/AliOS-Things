#include <iostream>
#include <stdio.h>
#include "utils.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/SimpleCredentialsProvider.h"
#include "alibabacloud/core/location/LocationClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;

using ::testing::_;
using ::testing::DefaultValue;

namespace {

  class mockLocationClient : public LocationClient {
   public:
    mockLocationClient(const string& key, const string& secret, const ClientConfiguration& cfg):
    LocationClient(key, secret, cfg) {}
    mockLocationClient(const Credentials& credentials, const ClientConfiguration& cfg):
    LocationClient(credentials, cfg) {}
    mockLocationClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration):
    LocationClient(credentialsProvider, configuration) {}
    MOCK_CONST_METHOD3(makeRequest, RpcServiceClient::JsonOutcome (const std::string &endpoint, const RpcServiceRequest &msg, HttpRequest::Method method));
  };

  TEST(LocationClient, sync_ak) {
    ClientConfiguration configuration("cn-hangzhou");
    Model::DescribeEndpointsRequest req;
    req.setId("cn-hangzhou");
    req.setServiceCode("ecs");
    req.setType("openAPI");

    mockLocationClient client("key", "secret", configuration);
    RpcServiceClient::JsonOutcome xout("{\"Endpoints\":{\"Endpoint\":[{\"Protocols\":{\"Protocols\":[\"HTTP\",\"HTTPS\"]},\"Type\":\"test-type\",\"Namespace\":\"test-namespace\",\"Id\":\"cn-hangzhou\",\"SerivceCode\":\"test-servicecode\",\"Endpoint\":\"test-endpoint\"}]},\"RequestId\":\"test-request-id\",\"Success\":true}");
    DefaultValue<RpcServiceClient::JsonOutcome>::Set(xout);
    EXPECT_CALL(client, makeRequest(_, _, _));

    LocationClient::DescribeEndpointsOutcome out = client.describeEndpoints(req);

    EXPECT_TRUE(out.error().errorCode().empty());
    EXPECT_TRUE(out.result().success());
    EXPECT_TRUE(out.result().requestId() == "test-request-id");
    EXPECT_TRUE(out.result().endpoints().size() == 1);

    Model::DescribeEndpointsResult::Endpoint ep = out.result().endpoints()[0];

    EXPECT_TRUE(ep.endpoint == "test-endpoint");
    EXPECT_TRUE(ep.id == "cn-hangzhou");
    EXPECT_TRUE(ep.namespace_ == "test-namespace");
    EXPECT_TRUE(ep.type == "test-type");
    EXPECT_TRUE(ep.protocols.size() == 2);
    std::vector<std::string>::iterator it;
    std::vector<std::string> protocols = ep.protocols;
    it = std::find(protocols.begin(), protocols.end(), string("HTTP"));
    EXPECT_TRUE(it != protocols.end());
    it = std::find(protocols.begin(), protocols.end(), string("HTTPS"));
    EXPECT_TRUE(it != protocols.end());
  }

  //{"Endpoints":{"Endpoint":[{"Protocols":{"Protocols":["HTTP","HTTPS"]},"Type":"openAPI","Namespace":"","Id":"cn-hangzhou","SerivceCode":"ecs","Endpoint":"ecs-cn-hangzhou.aliyuncs.com"}]},"RequestId":"test-request-id","Success":true}

  TEST(LocationClient, sync_credentials) {
    Credentials credentials("key", "secret");
    ClientConfiguration configuration("cn-hangzhou");

    Model::DescribeEndpointsRequest req;
    req.setId("cn-hangzhou");
    req.setServiceCode("ecs");
    req.setType("openAPI");

    mockLocationClient client(credentials, configuration);
    RpcServiceClient::JsonOutcome xout("{\"Endpoints\":{\"Endpoint\":[{\"Protocols\":{\"Protocols\":[\"HTTP\",\"HTTPS\"]},\"Type\":\"test-type\",\"Namespace\":\"test-namespace\",\"Id\":\"cn-hangzhou\",\"SerivceCode\":\"test-servicecode\",\"Endpoint\":\"test-endpoint\"}]},\"RequestId\":\"test-request-id\",\"Success\":true}");
    DefaultValue<RpcServiceClient::JsonOutcome>::Set(xout);
    EXPECT_CALL(client, makeRequest(_, _, _));

    LocationClient::DescribeEndpointsOutcome out = client.describeEndpoints(req);
    EXPECT_TRUE(out.error().errorCode().empty());
    EXPECT_TRUE(out.result().success());
    EXPECT_TRUE(out.result().requestId() == "test-request-id");
    EXPECT_TRUE(out.result().endpoints().size() == 1);

    Model::DescribeEndpointsResult::Endpoint ep = out.result().endpoints()[0];

    EXPECT_TRUE(ep.endpoint == "test-endpoint");
    EXPECT_TRUE(ep.id == "cn-hangzhou");
    EXPECT_TRUE(ep.namespace_ == "test-namespace");
    EXPECT_TRUE(ep.type == "test-type");
    EXPECT_TRUE(ep.protocols.size() == 2);
    std::vector<std::string>::iterator it;
    std::vector<std::string> protocols = ep.protocols;
    it = std::find(protocols.begin(), protocols.end(), string("HTTP"));
    EXPECT_TRUE(it != protocols.end());
    it = std::find(protocols.begin(), protocols.end(), string("HTTPS"));
    EXPECT_TRUE(it != protocols.end());
  }

  TEST(LocationClient, sync_credentials_ptr) {
    Credentials credentials("key", "secret");
    ClientConfiguration configuration("cn-hangzhou");

    Model::DescribeEndpointsRequest req;
    req.setId("cn-hangzhou");
    req.setServiceCode("ecs");
    req.setType("openAPI");

    mockLocationClient client(std::make_shared<SimpleCredentialsProvider>(credentials), configuration);
    RpcServiceClient::JsonOutcome xout("{\"Endpoints\":{\"Endpoint\":[{\"Protocols\":{\"Protocols\":[\"HTTP\",\"HTTPS\"]},\"Type\":\"test-type\",\"Namespace\":\"test-namespace\",\"Id\":\"cn-hangzhou\",\"SerivceCode\":\"test-servicecode\",\"Endpoint\":\"test-endpoint\"}]},\"RequestId\":\"test-request-id\",\"Success\":true}");
    DefaultValue<RpcServiceClient::JsonOutcome>::Set(xout);
    EXPECT_CALL(client, makeRequest(_, _, _));
    LocationClient::DescribeEndpointsOutcome out = client.describeEndpoints(req);
    EXPECT_TRUE(out.error().errorCode().empty());
    EXPECT_TRUE(out.result().success());
    EXPECT_TRUE(out.result().requestId() == "test-request-id");
    EXPECT_TRUE(out.result().endpoints().size() == 1);

    Model::DescribeEndpointsResult::Endpoint ep = out.result().endpoints()[0];

    EXPECT_TRUE(ep.endpoint == "test-endpoint");
    EXPECT_TRUE(ep.id == "cn-hangzhou");
    EXPECT_TRUE(ep.namespace_ == "test-namespace");
    EXPECT_TRUE(ep.type == "test-type");
    EXPECT_TRUE(ep.protocols.size() == 2);
    std::vector<std::string>::iterator it;
    std::vector<std::string> protocols = ep.protocols;
    it = std::find(protocols.begin(), protocols.end(), string("HTTP"));
    EXPECT_TRUE(it != protocols.end());
    it = std::find(protocols.begin(), protocols.end(), string("HTTPS"));
    EXPECT_TRUE(it != protocols.end());
  }

  TEST(LocationClient, callable) {
    InitializeSdk();
    ClientConfiguration configuration("cn-hangzhou");
    Model::DescribeEndpointsRequest req;
    req.setId("cn-hangzhou");
    req.setServiceCode("ecs");
    req.setType("openAPI");

    mockLocationClient client("key", "secret", configuration);
    RpcServiceClient::JsonOutcome xout("{\"Endpoints\":{\"Endpoint\":[{\"Protocols\":{\"Protocols\":[\"HTTP\",\"HTTPS\"]},\"Type\":\"test-type\",\"Namespace\":\"test-namespace\",\"Id\":\"cn-hangzhou\",\"SerivceCode\":\"test-servicecode\",\"Endpoint\":\"test-endpoint\"}]},\"RequestId\":\"test-request-id\",\"Success\":true}");
    DefaultValue<RpcServiceClient::JsonOutcome>::Set(xout);
    EXPECT_CALL(client, makeRequest(_, _, _));

    LocationClient::DescribeEndpointsOutcomeCallable cb = client.describeEndpointsCallable(req);
    LocationClient::DescribeEndpointsOutcome out = cb.get();

    EXPECT_TRUE(out.error().errorCode().empty());
    EXPECT_TRUE(out.result().success());
    EXPECT_TRUE(out.result().requestId() == "test-request-id");
    EXPECT_TRUE(out.result().endpoints().size() == 1);

    Model::DescribeEndpointsResult::Endpoint ep = out.result().endpoints()[0];

    EXPECT_TRUE(ep.endpoint == "test-endpoint");
    EXPECT_TRUE(ep.id == "cn-hangzhou");
    EXPECT_TRUE(ep.namespace_ == "test-namespace");
    EXPECT_TRUE(ep.type == "test-type");
    EXPECT_TRUE(ep.protocols.size() == 2);
    std::vector<std::string>::iterator it;
    std::vector<std::string> protocols = ep.protocols;
    it = std::find(protocols.begin(), protocols.end(), string("HTTP"));
    EXPECT_TRUE(it != protocols.end());
    it = std::find(protocols.begin(), protocols.end(), string("HTTPS"));
    EXPECT_TRUE(it != protocols.end());
    ShutdownSdk();
  }


void cb(const LocationClient *client,
          const Model::DescribeEndpointsRequest &req,
          const LocationClient::DescribeEndpointsOutcome& out,
          const std::shared_ptr<const AsyncCallerContext>& contex) {
    EXPECT_FALSE(out.error().errorCode().empty());
  }

  TEST(LocationClient, async) {
    InitializeSdk();

    ClientConfiguration configuration("cn-hangzhou");
    Model::DescribeEndpointsRequest req;
    req.setId("cn-hangzhou");
    req.setServiceCode("ecs");
    req.setType("openAPI");

    LocationClient client("key", "secret", configuration);
    LocationClient::DescribeEndpointsOutcome out;

    const AsyncCallerContext context;

    LocationClient::DescribeEndpointsAsyncHandler handler(cb);
    client.describeEndpointsAsync(req, handler, std::make_shared<const AsyncCallerContext>(context));
    ShutdownSdk();
  }

}
