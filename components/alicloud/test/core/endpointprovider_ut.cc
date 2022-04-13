#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "utils.h"
#include "alibabacloud/core/EndpointProvider.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;

using ::testing::_;
using ::testing::DefaultValue;


class mockEndpointProvider: public EndpointProvider {
 public:
  mockEndpointProvider(
      const Credentials &credentials,
      const ClientConfiguration &configuration,
      const std::string &regionId,
      const std::string &product,
      const std::string &serviceCode = std::string(),
      int durationSeconds = 3600
    ):
  EndpointProvider(credentials, configuration, regionId, product, serviceCode, durationSeconds) {}

  MOCK_CONST_METHOD1(describeEndpoints, LocationClient::DescribeEndpointsOutcome(const Model::DescribeEndpointsRequest &request));
};

TEST(EndpointProvider, basic_function) {
  const Credentials sub_user_credentials("key", "secret");
  ClientConfiguration config; // default is cn-hangzhou

  // user specified has 1st priority, even it is a wrong configuration
  config.setEndpoint("test-endpoint");
  mockEndpointProvider p0(sub_user_credentials, config, config.regionId(), "non-exist-product", "");
  EndpointProvider::EndpointOutcome out0 = p0.getEndpoint();
  EXPECT_TRUE(out0.result() == "test-endpoint");

  config.setEndpoint(""); // set endpoint to empty
  // invalid product
  mockEndpointProvider p1(sub_user_credentials, config, config.regionId(), "non-exist-Product", "");
  EndpointProvider::EndpointOutcome out1 = p1.getEndpoint();
  EXPECT_TRUE(out1.error().errorCode() == "InvalidRegionId");
  EXPECT_TRUE(out1.error().errorMessage() == "Product[non-exist-product] at region[cn-hangzhou] does not exist.");

  mockEndpointProvider p2(sub_user_credentials, config, "non-exist-region", "ecs", "");
  EndpointProvider::EndpointOutcome out2 = p2.getEndpoint();
  EXPECT_TRUE(out2.error().errorCode() == "InvalidRegionId");
  EXPECT_TRUE(out2.error().errorMessage() == "Product[ecs] at region[non-exist-region] does not exist.");

  // local_endpoints can provide the endpoint
  mockEndpointProvider p3(sub_user_credentials, config, config.regionId(), "ecs", "");
  EndpointProvider::EndpointOutcome out3 = p3.getEndpoint();
  EXPECT_TRUE(out3.result() == "ecs-cn-hangzhou.aliyuncs.com");

  // aegis has global_endpoint
  mockEndpointProvider p4(sub_user_credentials, config, config.regionId(), "aegis", "");
  EndpointProvider::EndpointOutcome out4 = p4.getEndpoint();
  EXPECT_TRUE(out4.result() == "aegis.cn-hangzhou.aliyuncs.com");


  // aegis has global_endpoint
  mockEndpointProvider p5(sub_user_credentials, config, config.regionId(), "Aegis", "");
  EndpointProvider::EndpointOutcome out5 = p5.getEndpoint();
  EXPECT_TRUE(out5.result() == "aegis.cn-hangzhou.aliyuncs.com");

  mockEndpointProvider p6(sub_user_credentials, config, "cn-shanghai", "ensdisk", "");
  EndpointProvider::EndpointOutcome out6 = p6.getEndpoint();
  EXPECT_TRUE(out6.error().errorCode() == "InvalidRegionId");
  EXPECT_TRUE(out6.error().errorMessage() == "Product[ensdisk] at region[cn-shanghai] does not exist.");
}

TEST(EndpointProvider, mock_remote) {
  const Credentials sub_user_credentials("key", "secret");
  ClientConfiguration config; // default is cn-hangzhou
  // config.setEndpoint("test-endpoint");

  mockEndpointProvider provider(sub_user_credentials, config, config.regionId(), "fake-ecs", "fake-ecs");

  Model::DescribeEndpointsRequest request;

  const string payload = "{\"RequestId\":\"test-request-id\",\"Success\":true,\"Endpoints\":{\"Endpoint\":[{\"Endpoint\":\"test-ep\",\"Id\":\"test-id\",\"Namespace\":\"test-namespace\",\"SerivceCode\":\"test-service-code\",\"Type\":\"test-type\",\"Protocols\":{\"Protocols\":[\"a\",\"b\"]}}]}}";
  Location::Model::DescribeEndpointsResult res(payload);
  LocationClient::DescribeEndpointsOutcome xout(res);

  DefaultValue<LocationClient::DescribeEndpointsOutcome>::Set(xout);
  EXPECT_CALL(provider, describeEndpoints(_));
  EndpointProvider::EndpointOutcome out = provider.getEndpoint();

  EXPECT_TRUE(out.error().errorCode().empty());
  EXPECT_TRUE(out.result() == "test-ep");
}

TEST(EndpointProvider, mock_remote_error) {
  const Credentials sub_user_credentials("key", "secret");
  ClientConfiguration config; // default is cn-hangzhou

  mockEndpointProvider provider(sub_user_credentials, config, config.regionId(), "fake-ecs", "fake-ecs");
  LocationClient::DescribeEndpointsOutcome xout(Error("any-error-code", "any-error-message"));

  DefaultValue<LocationClient::DescribeEndpointsOutcome>::Set(xout);
  EXPECT_CALL(provider, describeEndpoints(_));
  EndpointProvider::EndpointOutcome out = provider.getEndpoint();

  EXPECT_TRUE(out.error().errorCode() == "any-error-code");
}

TEST(EndpointProvider, serial_02) {
  utUtils utils;
  string key = utils.get_env("ENV_AccessKeyId");
  string secret = utils.get_env("ENV_AccessKeySecret");

  Credentials credentials(key, secret);
  ClientConfiguration configuration("cn-hangzhou");
  EndpointProvider ep(credentials, configuration, "cn-hangzhou", "arms", "arms");
  EndpointProvider::EndpointOutcome out = ep.getEndpoint();
  EXPECT_EQ("arms.cn-hangzhou.aliyuncs.com", out.result());

  EndpointProvider p1(credentials, configuration, "cn-shanghai", "fake-ecs", "fake-ecs");
  EndpointProvider::EndpointOutcome out1 = p1.getEndpoint();
  EXPECT_EQ("InvalidProduct", out1.error().errorCode());
  EXPECT_EQ("Prodcut[fake-ecs] does not exist.", out1.error().errorMessage());
}
