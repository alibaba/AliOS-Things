#include <iostream>
#include "../utils.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/cdn/CdnClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Cdn;

int main() {
  utUtils utils;
  string key = utils.get_env("ENV_AccessKeyId");
  string secret = utils.get_env("ENV_AccessKeySecret");
  InitializeSdk();
  ClientConfiguration configuration("cn-hangzhou");
  CdnClient client(key, secret, configuration);
  Model::DescribeCdnServiceRequest request;
  auto out = client.describeCdnService(request);
  cout << endl << "cdn describeCdnService result:" << endl;
  cout << "error code:         " << out.error().errorCode() << endl;
  cout << "changingChargeType: " << out.result().getChangingChargeType() << endl;
  cout << "internetChargeType: " << out.result().getInternetChargeType() << endl;
  cout << "instanceId:         " << out.result().getInstanceId() << endl;
  cout << "openingTime:        " << out.result().getOpeningTime() << endl;
  cout << "changingAffectTime: " << out.result().getChangingAffectTime() << endl << endl;
  ShutdownSdk();
}
