#include "gtest/gtest.h"
#include "alibabacloud/core/Error.h"

using namespace std;
using namespace AlibabaCloud;

TEST(Error, basic) {
  const std::string error_code = "errorCode";
  const std::string error_message = "errorMessage";
  const std::string host = "Host";
  const std::string request_id = "requestId";

  Error error(error_code, error_message);
  EXPECT_TRUE(error.errorCode() == error_code);
  EXPECT_TRUE(error.errorMessage() == error_message);
  EXPECT_TRUE(error.host() == "");
  EXPECT_TRUE(error.requestId() == "");

  error.setHost(host);
  error.setRequestId(request_id);
  EXPECT_TRUE(error.errorCode() == error_code);
  EXPECT_TRUE(error.errorMessage() == error_message);
  EXPECT_TRUE(error.host() == host);
  EXPECT_TRUE(error.requestId() == request_id);

  const string code1 = "invalid regionId";
  const string message1 = "invalid regionId cn-xxxx";
  error.setErrorCode(code1);
  error.setErrorMessage(message1);

  EXPECT_TRUE(error.errorCode() == code1);
  EXPECT_TRUE(error.errorMessage() == message1);

  const string detail = "any error detail";
  error.setDetail(detail);
  EXPECT_TRUE(error.detail() == detail);
}
