#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/location/model/DescribeEndpointsResult.h"

using namespace std;
using namespace AlibabaCloud;

TEST(DescribeEndpointsResult, basic) {
  const string payload = "{\"RequestId\":\"test-request-id\",\"Success\":true,\"Endpoints\":{\"Endpoint\":[{\"Endpoint\":\"test-ep\",\"Id\":\"test-id\",\"Namespace\":\"test-namespace\",\"SerivceCode\":\"test-service-code\",\"Type\":\"test-type\",\"Protocols\":{\"Protocols\":[\"a\",\"b\"]}}]}}";
  Location::Model::DescribeEndpointsResult res(payload);

  EXPECT_TRUE(res.success());
  EXPECT_TRUE(res.endpoints().size() == 1);
  EXPECT_TRUE(res.endpoints()[0].endpoint == "test-ep");
  EXPECT_TRUE(res.endpoints()[0].id == "test-id");
  EXPECT_TRUE(res.endpoints()[0].namespace_ == "test-namespace");
  EXPECT_TRUE(res.endpoints()[0].serivceCode == "test-service-code");
  EXPECT_TRUE(res.endpoints()[0].type == "test-type");
  EXPECT_TRUE(res.endpoints()[0].protocols.size() == 2);
  EXPECT_TRUE(res.endpoints()[0].protocols[0] == "a");
  EXPECT_TRUE(res.endpoints()[0].protocols[1] == "b");
}

TEST(DescribeEndpointsResult, set) {

  Location::Model::DescribeEndpointsResult res;

  res.setSuccess(true);
  EXPECT_TRUE(res.success());
  res.setSuccess(false);
  EXPECT_TRUE(res.success() == false);

  std::vector<std::string> protocols;
  protocols.push_back("p1");
  protocols.push_back("p2");
  Location::Model::DescribeEndpointsResult::Endpoint ep;
  ep.endpoint = "ep";
  ep.id = "id";
  ep.namespace_ = "ns";
  ep.serivceCode = "sc";
  ep.type = "t";
  ep.protocols = protocols;
  std::vector<Location::Model::DescribeEndpointsResult::Endpoint> endpoints_;
  endpoints_.push_back(ep);
  res.setEndpoints(endpoints_);
  res.setSuccess(true);
  EXPECT_TRUE(res.success());
  EXPECT_TRUE(res.endpoints().size() == 1);
  EXPECT_TRUE(res.endpoints()[0].endpoint == "ep");
  EXPECT_TRUE(res.endpoints()[0].id == "id");
  EXPECT_TRUE(res.endpoints()[0].namespace_ == "ns");
  EXPECT_TRUE(res.endpoints()[0].serivceCode == "sc");
  EXPECT_TRUE(res.endpoints()[0].type == "t");
  EXPECT_TRUE(res.endpoints()[0].protocols.size() == 2);
  EXPECT_TRUE(res.endpoints()[0].protocols[0] == "p1");
  EXPECT_TRUE(res.endpoints()[0].protocols[1] == "p2");
}
