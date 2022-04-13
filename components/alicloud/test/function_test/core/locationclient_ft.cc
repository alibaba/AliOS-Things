#include <iostream>
#include <stdio.h>
#include <algorithm>
#include "../utils.h"
#include "gtest/gtest.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/SimpleCredentialsProvider.h"
#include "alibabacloud/core/location/LocationClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;

namespace
{
utUtils utils;

const string key = utils.get_env("ENV_AccessKeyId");
const string secret = utils.get_env("ENV_AccessKeySecret");

TEST(LocationClient, basic)
{
  ClientConfiguration configuration("cn-hangzhou");
  Model::DescribeEndpointsRequest req;
  req.setId("cn-hangzhou");
  req.setServiceCode("ecs");
  req.setType("openAPI");

  LocationClient client(key, secret, configuration);

  LocationClient::DescribeEndpointsOutcome out = client.describeEndpoints(req);
  EXPECT_TRUE(out.error().errorCode() == "");
  EXPECT_TRUE(out.result().endpoints().size() == 1);
  Model::DescribeEndpointsResult::Endpoint ep = out.result().endpoints()[0];

  EXPECT_TRUE(ep.endpoint == "ecs-cn-hangzhou.aliyuncs.com");
  EXPECT_TRUE(ep.id == "cn-hangzhou");
  EXPECT_TRUE(ep.namespace_ == "26842" || ep.namespace_.empty());
  EXPECT_TRUE(ep.type == "openAPI");
  EXPECT_TRUE(ep.protocols.size() == 2);
  std::vector<std::string>::iterator it;
  std::vector<std::string> protocols = ep.protocols;

  it = std::find(protocols.begin(), protocols.end(), string("HTTP"));
  EXPECT_TRUE(it != protocols.end());

  it = std::find(protocols.begin(), protocols.end(), string("HTTPS"));
  EXPECT_TRUE(it != protocols.end());

  EXPECT_TRUE(out.result().success());
  const string req_id = "205302F3-6E1A-4D6F-82C5-2F165FB94038";
  EXPECT_TRUE(out.result().requestId().length() == req_id.length());
}

TEST(LocationClient, credentials)
{
  Credentials credentials(key, secret);
  ClientConfiguration configuration("cn-hangzhou");

  Model::DescribeEndpointsRequest req;
  req.setId("cn-hangzhou");
  req.setServiceCode("ecs");
  req.setType("openAPI");
  req.setMethod(HttpRequest::Post);

  LocationClient client(credentials, configuration);

  LocationClient::DescribeEndpointsOutcome out = client.describeEndpoints(req);
  EXPECT_TRUE(out.error().errorCode() == "");
  EXPECT_TRUE(out.result().endpoints().size() == 1);
  Model::DescribeEndpointsResult::Endpoint ep = out.result().endpoints()[0];

  EXPECT_TRUE(ep.endpoint == "ecs-cn-hangzhou.aliyuncs.com");
  EXPECT_TRUE(ep.id == "cn-hangzhou");
  EXPECT_TRUE(ep.namespace_ == "26842" || ep.namespace_.empty());
  EXPECT_TRUE(ep.type == "openAPI");
  EXPECT_TRUE(ep.protocols.size() == 2);
  std::vector<std::string>::iterator it;
  std::vector<std::string> protocols = ep.protocols;

  it = std::find(protocols.begin(), protocols.end(), string("HTTP"));
  EXPECT_TRUE(it != protocols.end());

  it = std::find(protocols.begin(), protocols.end(), string("HTTPS"));
  EXPECT_TRUE(it != protocols.end());

  EXPECT_TRUE(out.result().success());
  const string req_id = "205302F3-6E1A-4D6F-82C5-2F165FB94038";
  EXPECT_TRUE(out.result().requestId().length() == req_id.length());
}

TEST(LocationClient, credentials_another)
{
  Credentials credentials(key, secret);
  ClientConfiguration configuration("cn-hangzhou");
  configuration.setConnectTimeout(300000);
  configuration.setReadTimeout(300000);

  Model::DescribeEndpointsRequest req;
  req.setId("cn-hangzhou");
  req.setServiceCode("ecs");
  req.setType("openAPI");

  LocationClient client(std::make_shared<SimpleCredentialsProvider>(credentials), configuration);

  LocationClient::DescribeEndpointsOutcome out = client.describeEndpoints(req);
  EXPECT_EQ(out.error().errorCode(), "");
  EXPECT_EQ(out.result().endpoints().size(), 1);
  Model::DescribeEndpointsResult::Endpoint ep = out.result().endpoints()[0];
  EXPECT_TRUE(ep.endpoint == "ecs-cn-hangzhou.aliyuncs.com");
  EXPECT_TRUE(ep.id == "cn-hangzhou");
  EXPECT_TRUE(ep.namespace_ == "26842" || ep.namespace_.empty());
  EXPECT_TRUE(ep.type == "openAPI");
  EXPECT_TRUE(ep.protocols.size() == 2);
  std::vector<std::string>::iterator it;
  std::vector<std::string> protocols = ep.protocols;

  it = std::find(protocols.begin(), protocols.end(), string("HTTP"));
  EXPECT_TRUE(it != protocols.end());

  it = std::find(protocols.begin(), protocols.end(), string("HTTPS"));
  EXPECT_TRUE(it != protocols.end());

  EXPECT_TRUE(out.result().success());
  const string req_id = "205302F3-6E1A-4D6F-82C5-2F165FB94038";
  EXPECT_TRUE(out.result().requestId().length() == req_id.length());
}

TEST(LocationClient, callable)
{
  InitializeSdk();
  ClientConfiguration configuration("cn-hangzhou");
  configuration.setConnectTimeout(300000);
  configuration.setReadTimeout(300000);
  Model::DescribeEndpointsRequest req;
  req.setId("cn-hangzhou");
  req.setServiceCode("ecs");
  req.setType("openAPI");

  LocationClient client(key, secret, configuration);
  LocationClient::DescribeEndpointsOutcomeCallable cb = client.describeEndpointsCallable(req);
  LocationClient::DescribeEndpointsOutcome out = cb.get();

  EXPECT_EQ("", out.error().errorCode());
  EXPECT_EQ(1, out.result().endpoints().size());
  Model::DescribeEndpointsResult::Endpoint ep = out.result().endpoints()[0];
  EXPECT_TRUE(ep.endpoint == "ecs-cn-hangzhou.aliyuncs.com");
  EXPECT_TRUE(ep.id == "cn-hangzhou");
  EXPECT_TRUE(ep.namespace_ == "26842" || ep.namespace_.empty());
  EXPECT_TRUE(ep.type == "openAPI");
  EXPECT_TRUE(ep.protocols.size() == 2);
  std::vector<std::string>::iterator it;
  std::vector<std::string> protocols = ep.protocols;

  it = std::find(protocols.begin(), protocols.end(), string("HTTP"));
  EXPECT_TRUE(it != protocols.end());

  it = std::find(protocols.begin(), protocols.end(), string("HTTPS"));
  EXPECT_TRUE(it != protocols.end());

  EXPECT_TRUE(out.result().success());
  const string req_id = "205302F3-6E1A-4D6F-82C5-2F165FB94038";
  EXPECT_TRUE(out.result().requestId().length() == req_id.length());
  ShutdownSdk();
}

void cb(const LocationClient *client,
        const Model::DescribeEndpointsRequest &req,
        const LocationClient::DescribeEndpointsOutcome &out,
        const std::shared_ptr<const AsyncCallerContext> &contex)
{

  EXPECT_EQ(out.error().errorCode(), "");
  EXPECT_EQ(out.result().endpoints().size(), 1);
  Model::DescribeEndpointsResult::Endpoint ep = out.result().endpoints()[0];
  EXPECT_TRUE(ep.endpoint == "ecs-cn-hangzhou.aliyuncs.com");
  EXPECT_TRUE(ep.id == "cn-hangzhou");
  EXPECT_TRUE(ep.namespace_ == "26842" || ep.namespace_.empty());
  EXPECT_TRUE(ep.type == "openAPI");
  EXPECT_TRUE(ep.protocols.size() == 2);
  std::vector<std::string>::iterator it;
  std::vector<std::string> protocols = ep.protocols;

  it = std::find(protocols.begin(), protocols.end(), string("HTTP"));
  EXPECT_TRUE(it != protocols.end());

  it = std::find(protocols.begin(), protocols.end(), string("HTTPS"));
  EXPECT_TRUE(it != protocols.end());

  EXPECT_TRUE(out.result().success());
  const string req_id = "205302F3-6E1A-4D6F-82C5-2F165FB94038";
  EXPECT_TRUE(out.result().requestId().length() == req_id.length());
}

TEST(LocationClient, async)
{
  InitializeSdk();

  ClientConfiguration configuration("cn-hangzhou");
  Model::DescribeEndpointsRequest req;
  req.setId("cn-hangzhou");
  req.setServiceCode("ecs");
  req.setType("openAPI");

  LocationClient client(key, secret, configuration);
  LocationClient::DescribeEndpointsOutcome out;

  const AsyncCallerContext context;

  LocationClient::DescribeEndpointsAsyncHandler handler(cb);
  client.describeEndpointsAsync(req, handler, std::make_shared<const AsyncCallerContext>(context));
  ShutdownSdk();
}
} // namespace
