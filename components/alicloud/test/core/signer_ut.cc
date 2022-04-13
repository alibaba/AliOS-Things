
#include "gtest/gtest.h"
#include "alibabacloud/core/Signer.h"

using namespace std;
using namespace AlibabaCloud;

namespace {
  TEST(Signer, basic) {
    HmacSha1Signer sig;
    string sign = sig.generate("GET&%2F&AccessKeyId%3Dtestid%26Action%3DDescribeRegions"
        "%26Format%3DXML%26RegionId%3Dregion1%26SignatureMethod%3DHMAC-SHA1"
        "%26SignatureNonce%3DNwDAxvLU6tFE0DVb%26SignatureVersion%3D1.0"
        "%26TimeStamp%3D2012-12-26T10%253A33%253A56Z%26Version%3D2013-01-10",
        "testsecret&");
    EXPECT_TRUE("axE3FUHgDyfm9/+Iep0HpZXrRwE=" == sign);
  }

  TEST(Signer, unicode) {
    HmacSha1Signer sig;
    string sign = sig.generate("GET&%2F&AccessKeyId%3Dtestid%26Action%3DDescribeRegions"
        "%26TestUnicode%3DDescribeRegions啊啊啊"
        "%26Format%3DXML%26RegionId%3Dregion1%26SignatureMethod%3DHMAC-SHA1"
        "%26SignatureNonce%3DNwDAxvLU6tFE0DVb%26SignatureVersion%3D1.0"
        "%26TimeStamp%3D2012-12-26T10%253A33%253A56Z%26Version%3D2013-01-10",
        "testsecret&");
    EXPECT_TRUE("axE3FUHgDyfm9/+Iep0HpZXrRwE=" == sign);
  }
}
