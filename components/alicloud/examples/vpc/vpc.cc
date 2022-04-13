#include <iostream>
#include "../utils.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/vpc/VpcClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Vpc;

int main() {
  utUtils utils;
  string key = utils.get_env("ENV_AccessKeyId");
  string secret = utils.get_env("ENV_AccessKeySecret");
  InitializeSdk();
  ClientConfiguration configuration("cn-hangzhou");
  VpcClient client(key, secret, configuration);
  Model::DescribeNatGatewaysRequest request;
  request.setPageSize(10);
  auto outcome = client.describeNatGateways(request);
  cout << endl << "vpc describeNatGateways result" << endl;
  cout << "error code:  " << outcome.error().errorCode() << endl;
  cout << "totalCount:  " << outcome.result().getTotalCount() << endl << endl;
  ShutdownSdk();
}
