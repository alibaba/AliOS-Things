
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/ServiceRequest.h"
#include "alibabacloud/core/AlibabaCloud.h"

using namespace std;
using namespace AlibabaCloud;

namespace {
  class TestServiceRequest: public ServiceRequest {
   public:
    explicit TestServiceRequest(const std::string &product, const std::string &version):
      ServiceRequest(product, version)
    {}
    explicit TestServiceRequest(const ServiceRequest &other):
      ServiceRequest(other)
    {}
    explicit TestServiceRequest(ServiceRequest &&other):
      ServiceRequest(other)
    {}

    using ServiceRequest::addParameter;
    using ServiceRequest::coreParameter;
    using ServiceRequest::parameter;
    using ServiceRequest::removeParameter;
    using ServiceRequest::setBodyParameter;
    using ServiceRequest::setContent;
    using ServiceRequest::setCoreParameter;
    using ServiceRequest::setParameter;
    using ServiceRequest::setParameters;
    using ServiceRequest::setProduct;
    using ServiceRequest::setResourcePath;
    using ServiceRequest::setVersion;
  };

  TEST(ServiceRequest, basic) {

    TestServiceRequest sr1("ECS", "1.0");
    TestServiceRequest sr2(sr1);
    EXPECT_EQ(string("1.0"), sr1.getHeader(string("x-acs-version")));

    sr1.addParameter("k1", "v1");
    sr2.addParameter("k2", "v2");

    ServiceRequest::ParameterCollection p1 = sr1.parameters();
    ServiceRequest::ParameterCollection p2 = sr2.parameters();

    EXPECT_TRUE(sr1.parameter("k1") == "v1");
    sr1.addParameter("kx", "vx");
    EXPECT_TRUE(sr1.parameter("kx") == "vx");

    EXPECT_TRUE(sr1.parameters().size() == 2);

    sr1.removeParameter("k1");
    EXPECT_TRUE(sr1.parameters().size() == 1);

    sr1.setCoreParameter("kcore1", "vcore1");
    EXPECT_TRUE(sr1.parameter("kcore1") == "vcore1");
    EXPECT_TRUE(sr1.coreParameter("kcore1") == "vcore1");

    sr1.setParameter("ka", "va");
    EXPECT_TRUE(sr1.parameter("ka") == "va");
    EXPECT_TRUE(sr1.coreParameter("ka") == "va");

    sr1.setBodyParameter("name", "value");

    sr1.setContent("123456", 6);
    EXPECT_TRUE(sr1.contentSize() == 6);
    EXPECT_TRUE(sr1.hasContent() == true);
    EXPECT_TRUE(strncmp(sr1.content(), "123456", 6) == 0);

    sr1.setContent("aliyunsdk", 9);
    EXPECT_TRUE(sr1.contentSize() == 9);
    EXPECT_TRUE(sr1.hasContent() == true);
    EXPECT_TRUE(strncmp(sr1.content(), "aliyunsdk", 9) == 0);

    sr1.setProduct("OSS");
    EXPECT_TRUE(sr1.product() == "OSS");

    sr1.setVersion("3.0");
    EXPECT_TRUE(sr1.version() == "3.0");

    sr1.setResourcePath("http://abc.com/a/b/c");
    EXPECT_TRUE(sr1.resourcePath() == "http://abc.com/a/b/c");

    ServiceRequest::ParameterCollection ps;
    ps["km"] = "vm";
    ps["kn"] = "vn";
    sr1.setParameters(ps);
    ServiceRequest::ParameterCollection pc = sr1.parameters();
    EXPECT_TRUE(pc.at("km") == "vm");
    EXPECT_TRUE(pc.at("kn") == "vn");

    ServiceRequest::ParameterCollection bp = sr1.bodyParameters();
    EXPECT_EQ(bp.at("name"), "value");

    EXPECT_TRUE(sr1.connectTimeout() == kInvalidTimeout);
    EXPECT_TRUE(sr1.readTimeout() == kInvalidTimeout);
    sr1.setConnectTimeout(1234);
    sr1.setReadTimeout(22233);
    EXPECT_TRUE(sr1.connectTimeout() == 1234);
    EXPECT_TRUE(sr1.readTimeout() == 22233);
  }
}
