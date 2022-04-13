#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "alibabacloud/core/ServiceResult.h"


using namespace std;
using namespace AlibabaCloud;

namespace AlibabaCloud{
  class myServiceResult: public ServiceResult {
   public:
    myServiceResult(){}

    void setRequestId(const string request_id) {
      ServiceResult::setRequestId(request_id);
    }
  };
}

TEST(ServiceResult, basic) {
  ServiceResult* service_result = new ServiceResult();
  EXPECT_TRUE(service_result->requestId() == "");
  myServiceResult* sr = new myServiceResult();
  sr->setRequestId("testRequestId");
  EXPECT_TRUE(sr->requestId() == "testRequestId");
  delete(service_result);
  delete(sr);
}
