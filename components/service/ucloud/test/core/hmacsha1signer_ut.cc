#include "gtest/gtest.h"
#include "alibabacloud/core/HmacSha1Signer.h"
#include <openssl/hmac.h>

using namespace std;
using namespace AlibabaCloud;

namespace {

TEST(HmacSha1Signer, assign) {

    HmacSha1Signer sig;
    string sign = sig.generate("GET&%2F&AccessKeyId%3Dtestid%26Action%3DDescribeRegions"
        "%26Format%3DXML%26RegionId%3Dregion1%26SignatureMethod%3DHMAC-SHA1"
        "%26SignatureNonce%3DNwDAxvLU6tFE0DVb%26SignatureVersion%3D1.0"
        "%26TimeStamp%3D2012-12-26T10%253A33%253A56Z%26Version%3D2013-01-10",
        "testsecret&");
    EXPECT_TRUE("axE3FUHgDyfm9/+Iep0HpZXrRwE=" == sign);
    EXPECT_TRUE(sig.type() == HmacSha1Signer::HmacSha1);
    EXPECT_TRUE(sig.name() == "HMAC-SHA1");
    EXPECT_TRUE(sig.version() == "1.0");
  }

  TEST(HmacSha1Signer, edge_from_empty_string) {
    HmacSha1Signer sign;
    EXPECT_TRUE(sign.generate("", "") == "");
  }

  TEST(HmacSha1Signer, unicode) {
    HmacSha1Signer sig;
    string sign = sig.generate("GET&%2F&AccessKeyId%3Dtestid%26Action%3DDescribeRegions"
        "%26TestUnicode%3DDescribeRegions啊啊啊"
        "%26Format%3DXML%26RegionId%3Dregion1%26SignatureMethod%3DHMAC-SHA1"
        "%26SignatureNonce%3DNwDAxvLU6tFE0DVb%26SignatureVersion%3D1.0"
        "%26TimeStamp%3D2012-12-26T10%253A33%253A56Z%26Version%3D2013-01-10",
        "testsecret&");
    EXPECT_TRUE("VJ1a5YBLy+shtBK/l97MCMq+mzs=" == sign);
  }

  TEST(HmacSha1Signer, specialQuery) {
    HmacSha1Signer sig;
    string sign = sig.generate("GET&%2F&AccessKeyId%3Dtestid%26Action%3DDescribeRegions"
        "%26TestUnicode%3DDescri&beRegions"
        "%26Format%3DXML%26RegionId%3Dregion1%26SignatureMethod%3DHMAC-SHA1"
        "%26SignatureNonce%3DNwDAxvLU6tFE0DVb%26SignatureVersion%3D1.0"
        "%26TimeStamp%3D2012-12-26T10%253A33%253A56Z%26Version%3D2013-01-10",
        "testsecret&");
    EXPECT_TRUE("3PuleV7r01lLr4wwvWNCOuLKV5A=" == sign);
  }
}