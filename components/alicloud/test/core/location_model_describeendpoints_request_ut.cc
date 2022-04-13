#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/location/model/DescribeEndpointsRequest.h"

using namespace std;
using namespace AlibabaCloud;

TEST(DescribeEndpointsRequest, basic) {
  Location::Model::DescribeEndpointsRequest req;

  req.setServiceCode("test-sc");
  req.setId("test-id");
  req.setType("test-type");

  EXPECT_TRUE(req.serviceCode() == "test-sc");
  EXPECT_TRUE(req.id() == "test-id");
  EXPECT_TRUE(req.type() == "test-type");
}

