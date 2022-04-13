#include "gtest/gtest.h"
#include "alibabacloud/core/AsyncCallerContext.h"

using namespace std;
using namespace AlibabaCloud;


TEST(AsyncCallerContext, basic) {
  const std::string sys_generated_uuid = "643ce9ff-32b5-444c-814a-153b87a6aca8";
  const std::string init_uuid = "init_uuid_for_test";
  const std::string new_uuid = "new_uuid_for_test";
  AsyncCallerContext context(init_uuid);
  EXPECT_TRUE(context.uuid() == init_uuid);
  context.setUuid(new_uuid);
  EXPECT_TRUE(context.uuid() == new_uuid);
  AsyncCallerContext context_from_GenerateUuid = AsyncCallerContext();
  EXPECT_TRUE(context_from_GenerateUuid.uuid().length() == sys_generated_uuid.length());
}
