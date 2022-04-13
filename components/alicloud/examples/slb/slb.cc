#include <iostream>
#include "../utils.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/slb/SlbClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Slb;

int main() {
  utUtils utils;
  string key = utils.get_env("ENV_AccessKeyId");
  string secret = utils.get_env("ENV_AccessKeySecret");
  InitializeSdk();
  ClientConfiguration configuration("cn-hangzhou");
  SlbClient client(key, secret, configuration);
  Model::DescribeLoadBalancersRequest request;
  request.setPageSize(10);
  auto outcome = client.describeLoadBalancers(request);
  cout << endl << "slb describeLoadBalancers result" << endl;
  cout << "error code:  " << outcome.error().errorCode() << endl;
  cout << "totalCount:  " << outcome.result().getTotalCount() << endl << endl;
  ShutdownSdk();
}
