#include <iostream>
#include <cstdlib>
#include <cstring>
#include "../utils.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/CommonClient.h"

using namespace std;
using namespace AlibabaCloud;

int main(int argc, char** argv) {
  utUtils utils;
  string key = utils.get_env("ENV_AccessKeyId");
  string secret = utils.get_env("ENV_AccessKeySecret");


  InitializeSdk();
  ClientConfiguration configuration("cn-shanghai");
  CommonClient client(key, secret, configuration);

  CommonRequest request(CommonRequest::RoaPattern);
  request.setScheme("http");
  request.setDomain("nlp.cn-shanghai.aliyuncs.com");
  request.setResourcePath("/nlp/api/wordsegment/general");
  request.setHttpMethod(HttpRequest::Post);

  // if windows, Chinese character will result in build error.
  const std::string data = "{\"lang\":\"ZH\",\"text\":\"Iphone is a good choice.\"}";

  request.setContent(data.c_str(), data.size());
  request.setHeaderParameter("Content-Type", "application/json;chrset=utf-8");
  request.setHeaderParameter("Accept", "application/json");
  request.setVersion("2018-04-08");

  auto out = client.commonResponse(request);
  if (!out.isSuccess()) {
    cout << "error code:      " << out.error().errorCode() << endl;
    cout << "error message:   " << out.error().errorMessage() << endl;
    cout << "error host:      " << out.error().host() << endl;
    cout << "error requestId: " << out.error().requestId() << endl;
    cout << "error detail:    " << out.error().detail() << endl;
    ShutdownSdk();
    return -1;
  }
  cout << endl << "wordsegment retruns: " << out.result().payload() << std::endl << endl;
  ShutdownSdk();
  return 0;
}
