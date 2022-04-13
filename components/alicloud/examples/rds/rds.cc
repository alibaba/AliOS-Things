#include <iostream>
#include "../utils.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/rds/RdsClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Rds;

int main() {
  utUtils utils;
  string key = utils.get_env("ENV_AccessKeyId");
  string secret = utils.get_env("ENV_AccessKeySecret");
  InitializeSdk();
  ClientConfiguration configuration("cn-hangzhou");
  RdsClient client(key, secret, configuration);
  Model::DescribeDBInstancesRequest request;
  request.setPageSize(10);
  auto outcome = client.describeDBInstances(request);
  cout << endl << "rds describeDBInstances result" << endl;
  cout << "error code:          " << outcome.error().errorCode() << endl;
  cout << "getTotalRecordCount: " << outcome.result().getTotalRecordCount() << endl << endl;
  ShutdownSdk();
}
