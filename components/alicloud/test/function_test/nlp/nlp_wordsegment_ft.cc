#include <iostream>
#include <cstdlib>
#include <cstring>
#include "gtest/gtest.h"
#include "../utils.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/CommonClient.h"
#include "alibabacloud/core/HttpRequest.h"
using namespace std;
using namespace AlibabaCloud;

namespace {
  TEST(nlp, wordsegment) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    AlibabaCloud::InitializeSdk();
    ClientConfiguration configuration("cn-shanghai");

    CommonClient client(key, secret, configuration);
    // create request and assign parameters
    CommonRequest request(CommonRequest::RoaPattern);
    request.setScheme("http");
    request.setDomain("nlp.cn-shanghai.aliyuncs.com");
    request.setResourcePath("/nlp/api/wordsegment/general");
    request.setHttpMethod(HttpRequest::Post);
    const char * data = "{\"lang\":\"ZH\",\"text\":\"Iphone is a good choice 专用数据线\"}";
    request.setContent(data, strlen(data));
    request.setHeaderParameter("Content-Type", "application/json;chrset=utf-8");
    request.setVersion("2018-04-08");
    auto outcome = client.commonResponse(request);
    EXPECT_TRUE(outcome.error().errorCode().empty());
    const std::string expected = "{\"data\":[{\"id\":0,\"word\":\"Iphone\"},{\"id\":1,\"word\":\" \"},{\"id\":2,\"word\":\"is\"},{\"id\":3,\"word\":\" \"},{\"id\":4,\"word\":\"a\"},{\"id\":5,\"word\":\" \"},{\"id\":6,\"word\":\"good\"},{\"id\":7,\"word\":\" \"},{\"id\":8,\"word\":\"choice\"},{\"id\":9,\"word\":\" \"},{\"id\":10,\"word\":\"专用\"},{\"id\":11,\"word\":\"数据线\"}]}";
    EXPECT_TRUE(outcome.result().payload() == expected);
    AlibabaCloud::ShutdownSdk();
  }

  TEST(nlp, wordsegment_error) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    AlibabaCloud::InitializeSdk();
    ClientConfiguration configuration("cn-shanghai");

    CommonClient client(key, secret, configuration);
    // create request and assign parameters
    CommonRequest request(CommonRequest::RoaPattern);
    request.setScheme("http");
    request.setDomain("nlp.cn-shanghai.aliyuncs.com");
    request.setResourcePath("/nlp/api/wordsegment/general");
    request.setHttpMethod(HttpRequest::Post);
    const char * data = "invlaid text";
    request.setContent(data, strlen(data));
    request.setHeaderParameter("Content-Type", "application/json;chrset=utf-8");
    request.setVersion("2018-04-08");
    auto outcome = client.commonResponse(request);
    const string error = "{\"errorCode\":10007,\"errorMsg\":\"body json format invalid\"}";
    EXPECT_TRUE(outcome.error().detail() == error);
    AlibabaCloud::ShutdownSdk();
  }

  TEST(nlp, wordsegmentClientTimeout) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    AlibabaCloud::InitializeSdk();
    ClientConfiguration configuration("cn-shanghai");
    configuration.setConnectTimeout(1);
    configuration.setReadTimeout(123);
    CommonClient client(key, secret, configuration);
    // create request and assign parameters
    CommonRequest request(CommonRequest::RoaPattern);
    request.setScheme("http");
    request.setDomain("nlp.cn-shanghai.aliyuncs.com");
    request.setResourcePath("/nlp/api/wordsegment/general");
    request.setHttpMethod(HttpRequest::Post);
    const char * data = "{\"lang\":\"ZH\",\"text\":\"Iphone is a good choice 专用数据线\"}";
    request.setContent(data, strlen(data));
    request.setHeaderParameter("Content-Type", "application/json;chrset=utf-8");
    request.setVersion("2018-04-08");

    auto outcome = client.commonResponse(request);
    EXPECT_TRUE(outcome.error().errorCode() == "OperationTimeoutError");
    EXPECT_TRUE(outcome.error().errorMessage().find("Timeout (connectTimeout: 1ms, readTimeout: 123ms) when connect or read") == 0);
    AlibabaCloud::ShutdownSdk();
  }


  TEST(nlp, wordsegmentRequestTimeout) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    AlibabaCloud::InitializeSdk();
    ClientConfiguration configuration("cn-shanghai");

    CommonClient client(key, secret, configuration);
    // create request and assign parameters
    CommonRequest request(CommonRequest::RoaPattern);
    request.setScheme("http");
    request.setDomain("nlp.cn-shanghai.aliyuncs.com");
    request.setConnectTimeout(11);
    request.setReadTimeout(213);
    request.setResourcePath("/nlp/api/wordsegment/general");
    request.setHttpMethod(HttpRequest::Post);
    const char * data = "{\"lang\":\"ZH\",\"text\":\"Iphone is a good choice 专用数据线\"}";
    request.setContent(data, strlen(data));
    request.setHeaderParameter("Content-Type", "application/json;chrset=utf-8");
    request.setVersion("2018-04-08");

    auto outcome = client.commonResponse(request);
    EXPECT_TRUE(outcome.error().errorCode() == "OperationTimeoutError");
    EXPECT_TRUE(outcome.error().errorMessage().find("Timeout (connectTimeout: 11ms, readTimeout: 213ms) when connect or read") == 0);
    AlibabaCloud::ShutdownSdk();
  }

  TEST(nlp, wordsegmentClientRequestTimeout) {
    utUtils utils;
    string key = utils.get_env("ENV_AccessKeyId");
    string secret = utils.get_env("ENV_AccessKeySecret");

    AlibabaCloud::InitializeSdk();
    ClientConfiguration configuration("cn-shanghai");
    configuration.setConnectTimeout(1);
    configuration.setReadTimeout(123);
    CommonClient client(key, secret, configuration);
    // create request and assign parameters
    CommonRequest request(CommonRequest::RoaPattern);
    request.setScheme("http");
    request.setDomain("nlp.cn-shanghai.aliyuncs.com");
    request.setConnectTimeout(11);
    request.setReadTimeout(213);
    request.setResourcePath("/nlp/api/wordsegment/general");
    request.setHttpMethod(HttpRequest::Post);
    const char * data = "{\"lang\":\"ZH\",\"text\":\"Iphone is a good choice 专用数据线\"}";
    request.setContent(data, strlen(data));
    request.setHeaderParameter("Content-Type", "application/json;chrset=utf-8");
    request.setVersion("2018-04-08");

    auto outcome = client.commonResponse(request);
    EXPECT_TRUE(outcome.error().errorCode() == "OperationTimeoutError");
    EXPECT_TRUE(outcome.error().errorMessage().find("Timeout (connectTimeout: 11ms, readTimeout: 213ms) when connect or read") == 0);
    AlibabaCloud::ShutdownSdk();
  }
}
