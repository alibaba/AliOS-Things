#include <iostream>
#include "../utils.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/ecs/EcsClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Ecs;

int main() {
  utUtils utils;
  string key = utils.get_env("ENV_AccessKeyId");
  string secret = utils.get_env("ENV_AccessKeySecret");
  InitializeSdk();
  ClientConfiguration configuration("cn-hangzhou");
  EcsClient client(key, secret, configuration);
  Model::DescribeInstancesRequest request;
  request.setPageSize(10);
  auto outcome = client.describeInstances(request);
  cout << endl << "ecs describeInstances returned:" << endl;
  cout << "error code: " << outcome.error().errorCode() << endl;
  cout << "totalCount: " << outcome.result().getTotalCount() << endl << endl;

  ShutdownSdk();
}
